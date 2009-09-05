/*
    *
	* Copyright (c) 2009
	* vol4ok <vol4ok@highsecure.ru> PGP key ID - 0x7A1C8BB4A0F34B67
    *

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ntifs.h>
#include "defines.h"
#include "misc.h"
#include "code_hook.h"
#include "ldasm.h"

static ULONG locked;
static ULONG inp_dpcs;
static ULONG out_dpcs;
static ULONG dpc_refs;

static LIST_ENTRY	hook_descriptor_list;
static KSPIN_LOCK	hook_descriptor_lock;

static void global_lock_proc(PKDPC dpc, PKDPC *cpu_dpc, void *arg1, void *arg2)
{
	_disable();
	lock_dec(&inp_dpcs);
	
	while (locked != 0) { 
		KeStallExecutionProcessor(1);	
	}
	_enable();
	
	if (lock_dec(&dpc_refs) == 0) {
		ExFreePool(cpu_dpc);
	}	
	lock_dec(&out_dpcs);
}

KIRQL global_lock()
{
	KIRQL old_irql;
	PKDPC cpu_dpc;
	CCHAR n_cpu, i;
	
	n_cpu    = KeNumberProcessors;
	cpu_dpc  = ExAllocatePool(NonPagedPoolMustSucceed, sizeof(KDPC) * n_cpu);
	old_irql = KeRaiseIrqlToDpcLevel();
	locked   = 1;
	inp_dpcs = n_cpu - 1;
	out_dpcs = inp_dpcs;
	dpc_refs = inp_dpcs;
	
	for (i = 0; i < KeNumberProcessors; i++)
	{
		if (i != KeGetCurrentProcessorNumber()) 
		{
			KeInitializeDpc(&cpu_dpc[i], global_lock_proc, cpu_dpc);
			KeSetTargetProcessorDpc(&cpu_dpc[i], i);
			KeSetImportanceDpc(&cpu_dpc[i], HighImportance);
			KeInsertQueueDpc(&cpu_dpc[i], NULL, NULL);
		}
	}
	while (inp_dpcs != 0) { 
		KeStallExecutionProcessor(1); 
	}	
	_disable();
	
	return old_irql;
}

void global_unlock(KIRQL old_irql)
{
	locked = 0;
	
	while (out_dpcs != 0) { 
		KeStallExecutionProcessor(1); 
	}
	_enable();
	
	KeLowerIrql(old_irql);
}

hook_descriptor* hot_patch(void *src_proc, void *new_proc, void **old_proc)
{
	u8		*src_addr = src_proc;
	u8		*dst_addr = new_proc;
	u64		cr0;
	hook_descriptor	*hook_desc = NULL;
	
	if (!new_proc || !src_proc || !old_proc)
		return NULL;
	
	if ( (hook_desc = mem_alloc(sizeof(hook_descriptor))) == NULL )
		return NULL;
	
	/* check hot patch ability */
	if ( (p8(src_addr-5)[0] == 0x90 && p32(src_addr-4)[0] == 0x90909090) ||
		 (p8(src_addr-5)[0] == 0xCC && p32(src_addr-4)[0] == 0xCCCCCCCC) )
	{
		if (p16(src_addr)[0] == 0xFF8B){
			/* set original proc address */
			*old_proc = &src_addr[2];
		} else {
			u32  cmd_len;
			u8  *opcd;
			
			/* if size of first command != 2 byte - return */
			cmd_len = size_of_code(src_addr, &opcd);
			if ( cmd_len != 2 ){
				mem_free(hook_desc);
				return NULL;
			}
			
			/* alloc buffer */
			if ( (dst_addr = mem_alloc(16)) == NULL ){
				mem_free(hook_desc);
				return NULL;
			}

			*old_proc = dst_addr;
			memcpy(dst_addr, src_addr, cmd_len);
			dst_addr += cmd_len;
			src_addr += cmd_len;
		
			/* set JMP to original proc */
			set_jump(dst_addr,src_addr);

			src_addr = src_proc;
			dst_addr = new_proc;
		}
		
		cr0 = mem_open();
		
		/* set JMP new_proc */
		set_jump(src_addr-5,dst_addr);	
		
		/* set JMP SHORT $-5 */
		p16(src_addr)[0] = 0xF9EB;
		
		mem_close(cr0);
		
		zeromem(hook_desc,sizeof(hook_descriptor));
		hook_desc->hook_type = HOOK_TYPE_HOT_PATCH;
		hook_desc->src_proc = src_proc;
		hook_desc->old_proc = *old_proc;
		hook_desc->new_proc = new_proc;
		hook_desc->old_code_size = 2;
		
		return hook_desc;
	} else if ( p16(src_addr)[0] == 0xF9EB &&
				p8(src_addr-5)[0] == 0xE9 )
	{
		/* set original proc address */
		lock_xchg(p32(old_proc), p32(src_addr-4)[0]);
		*old_proc = addof(src_addr,(u32)*old_proc);
		
		cr0 = mem_open();
		
		/* set new JMP new_proc */
		lock_xchg(p32(src_addr-4), dst_addr - src_addr);
		
		zeromem(hook_desc,sizeof(hook_descriptor));
		hook_desc->hook_type = HOOK_TYPE_DOUBLE_HOT_PATCH;
		hook_desc->src_proc = src_proc;
		hook_desc->old_proc = *old_proc;
		hook_desc->new_proc = new_proc;
		hook_desc->old_code_size = 0;
		
		mem_close(cr0);
		
		return hook_desc;
	}
	
	return NULL;
}

hook_descriptor* hook_code(void *src_proc, void *new_proc, void **old_proc)
{
	u8  *src_addr;
	u8  *dst_addr;
	u32  cmd_len, flags;
	u32  all_len  = 0;
	u8  *opcd, *buf_addr;
	u64  cr0;
	KIRQL	irql;
	hook_descriptor	*hook_desc = NULL;
	
	if (!new_proc || !src_proc || !old_proc){
		return NULL;
	}
	
	if ( (hook_desc = mem_alloc(sizeof(hook_descriptor))) == NULL )
		return NULL;

	if ( (buf_addr = mem_alloc(64)) == NULL ) {
		mem_free(hook_desc);
		return NULL;
	}

	dst_addr = buf_addr; src_addr = src_proc;

	do
	{
		cmd_len = size_of_code(src_addr, &opcd);
		flags   = x_code_flags(src_addr);

		if (cmd_len == 0 || flags & OP_REL8)
		{
			mem_free(hook_desc);
			mem_free(buf_addr);
			return NULL;
		}

		memcpy(dst_addr, src_addr, cmd_len);

		if (flags & OP_REL32)
		{
			if (flags & OP_EXTENDED) {
				p32(dst_addr+2)[0] += src_addr - dst_addr;
			} else {
				p32(dst_addr+1)[0] += src_addr - dst_addr;
			}
		}

		src_addr += cmd_len;
		dst_addr += cmd_len;
		all_len  += cmd_len;
	} while (all_len < 5);

	dst_addr[0]            = 0xE9;
	p32(dst_addr+1)[0]     = src_addr - dst_addr - 5;
	
	*old_proc = buf_addr;
	src_addr = src_proc; 
	dst_addr = new_proc;
	
	irql = global_lock();
	cr0 = mem_open();
	
	set_jump(src_addr,dst_addr);
	
	mem_close(cr0);
	global_unlock(irql);
	
	zeromem(hook_desc, sizeof(hook_descriptor));
	hook_desc->hook_type = HOOK_TYPE_SPLICING;
	hook_desc->src_proc = src_proc;
	hook_desc->old_proc = *old_proc;
	hook_desc->new_proc = new_proc;
	hook_desc->old_code_size = all_len;

	return hook_desc;
}

int set_hook(void *src_proc, void *new_proc, void **old_proc)
{
	hook_descriptor *hook_desc;
	
	DbgMsg("set_hook: 0x%p, 0x%p, 0x%p\n",src_proc, new_proc, old_proc);
	
	hook_desc = hot_patch(src_proc, new_proc, old_proc);
	if ( hook_desc ){
		insert_list_tail(&hook_descriptor_list, hook_desc, &hook_descriptor_lock);
		DbgMsg("Insert to hook list 0x%p\n", hook_desc);
		return 1;
	}
	
	hook_desc = hook_code(src_proc, new_proc, old_proc);
	if ( hook_desc ){
		insert_list_tail(&hook_descriptor_list, hook_desc, &hook_descriptor_lock);
		DbgMsg("Insert to hook list 0x%p\n", hook_desc);
		return 1;
	}
	
	return 0;
}
		
int init_hook_list()
{
	DbgMsg("Hook list init\n");
	init_list(&hook_descriptor_list);
	KeInitializeSpinLock(&hook_descriptor_lock);
	return 1;
}
