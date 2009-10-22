/*
*
* Copyright (c) october 2009
* Vladimir <progopis@jabber.ru> PGP key ID - 0x59CF2D8A75CB8417
*

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <windows.h>
#include <stdio.h>
#include "disasm.h"
#include "defines.h"
#include "vm.h"

namespace vm
{
	int push (vm_main::stack ** pstack, reg_size * ESP, stackvar var)
	{
		vm_main::stack * p;
		vm_main::stack * stk = *pstack;

		p = (vm_main::stack *)malloc(sizeof(vm_main::stack));
		p->var.type = var.type;
		p->var.reg_type = var.reg_type;
		switch (var.type)
		{
		case TYPE_DBG_REG:
		case TYPE_GEN_REG:
		case TYPE_SEG_REG:
			p->var.reg_val = var.reg_val;
		case TYPE_NUM:
			p->var = var.val;
			break;
		default:
			fprintf(stderr, "Fatal error! Unknown type!");
			return -1;
			break;
		}
		if ((*ESP) > 0)
			p->next = stk;
		else
			p->next = NULL;
		*pstack = p;
		(*ESP)++;

		return 0;
	}

	int pop (vm_main::stack ** stack, reg_size * ESP, stackvar * var)
	{
		vm_main::stack * p;
		vm_main::stack * stk = *stack;

		if (!(*ESP))
		{
			fprintf(stderr, "Fatal error! Out of stack!");
			return -2;
		}

		p = stk->next;
		var->type = stk->var.type;
		switch (var->type)
		{
		case TYPE_DBG_REG:
		case TYPE_GEN_REG:
		case TYPE_SEG_REG:
			var->reg_val = p->var.reg_val;
		case TYPE_NUM:
			var->val = p->var.val;
			break;
		default:
			fprintf(stderr, "Fatal error! Unknown type!");
			return -1;
			break;
		}
		free(stk);
		*stack = p;
		(*ESP)--;

		return 0;
	}
	int vm_main::execute (struct INSTRUCTION instr)
	{
		u64 groups;
		u16 id;
		stackvar var;
		reg_size * ESP;

		groups = instr.groups;
		id = instr.id;		

		if (groups & GRP_STACK)
			ESP = &(vm_main::registers.general[STACK_REG_INDEX]);

		if (groups & GRP_FLGCTRL)
		{
			switch (id)
			{
			// GRP_STACK
			case ID_PUSHF:
				// TODO: записать значение регистра флагов в var
				push (&stk, ESP, var);
			case ID_POPF:
				pop (&stk, ESP, &var);
				// сохранить в регистр флагов значение var
				break;
			// GRP_DATAMOV
			case ID_SAHF:
				break;
			case ID_LAHF:
				break;
			// TODO:
			//case ID_CMC, ID_CLC, ID_STC, ID_CLI, ID_STI, ID_CLD, ID_STD
			default:
				fprintf(stderr, "Fatal error! Unknown GRP_FLGCTRL instruction!");
				return -1;
				break;
			}
		}
		else
		{

		}

		return 0;
	}
	int vm_main::init (unsigned char block[])
	{
		int len;
		uint8_t *ptr, *end;
		uint8_t sf_prefixes[MAX_INSTRUCTION_LEN];
		struct INSTRUCTION instr;
		struct PARAMS params;
		char output[70];
		int reallen;
		// names
		char RegNames1[GEN_REGS_NUM][4] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
		memcpy(RegNames, RegNames1, sizeof(RegNames));
		// виртуальное выполнение с целью выделить эффективный код
		// разбить эффективный код на граф
		// задача: обойти возможную анти-отладку
		ptr = block;
		end = block + sizeof(block);

		params.arch = ARCH_COMMON;
		params.sf_prefixes = sf_prefixes;
		params.mode = DISASSEMBLE_MODE_32;

		while(ptr < end)
		{
			len = disassemble(ptr, &instr, &params);
			if (!len)
				printf("Fail.\n");
			else
			{
				reallen = dump(&instr, output, 70, OPTION_IMM_UHEX | OPTION_DISP_HEX);
				if (instr.groups & GRP_BRANCH)
				{
					// херачим разветление графа если ветка не условная
					// в случае call сделать 
				}
				else if (instr.groups & GRP_DATAMOV)
				{
					// детектим метаморф/полиморф
				}
				execute(instr); // виртуальное выполнение кода
				output[reallen] = '\n';
				output[reallen + 1] = 0;
				printf(output);
			}
			ptr += len;
		}
		// проверить граф на 

		return 0;
	}
	int vm_main::deobfuscate_nodes()
	{

		return 0;
	}
} // namespace vm
