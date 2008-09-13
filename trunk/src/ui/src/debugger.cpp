/*
    *    
    * Copyright (c) 2008 
    * d1mk4 <d1mk4@bk.ru> 
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

#include "precomp.h"
#include "debugger.h"

#define SYNC EnterCriticalSection(&sync);
#define UNSYNC LeaveCriticalSection(&sync);

Debugger::Debugger()
 : m_hitcount(0),
   m_sesId(0),
   m_trace_mode(0)
{
}

int Debugger::init()
{
	return trc_init();
}

void Debugger::bp_list()
{
	PBP_LST bp_list=trc_get_bp_list(m_sesId,NULL);
	CHAR hw[]="hardware";
	CHAR sf[]="software";
	PCHAR type=NULL;

	if(bp_list) {
		printf("bp list==========\n");
		for(int i=0;i<bp_list->count;i++) {
			switch(bp_list->bp[i].type) {
				case TRC_BP_SOFTWARE:
					type = sf;
				break;
				case TRC_HWBP_READWRITE:
				case TRC_HWBP_WRITE:
				case TRC_HWBP_EXECUTE:
					type = hw;
					break;
			}
			printf("addr=%#x\tTID=%#x type=%s\n",bp_list->bp[i].addr,bp_list->bp[i].TID,type);
		}
		printf("===================\n");
		trc_free_list(bp_list);
	}
}

void Debugger::threads_list()
{
PTHREAD_LIST thrd_lst=trc_get_thread_list(m_sesId);
		if(thrd_lst){
			printf("running threads list==================\n");
			for(int i=0;i<thrd_lst->count;i++){
				printf("TID=%x,teb addr=%x\n",thrd_lst->thread[i].TID,thrd_lst->thread[i].teb_addr);
			}
			printf("======================================\n");
			trc_free_list(thrd_lst);
		}
}

void Debugger::mod_list()
{
	PMOD_LIST mod_list=trc_get_module_list(m_sesId);
		if(mod_list){
			printf("loaded modules list==================\n");
			for(int i=0;i<mod_list->count;i++){
				printf("mod path: %s,mod base: %x,mod size: %x\n",mod_list->module[i].ModName,mod_list->module[i].ImageBase,mod_list->module[i].ImageSize);
			}
			printf("=====================================\n");
			trc_free_list(mod_list);
		}
}

ULONG Debugger::set_hwbp(char * bp)
{
	ULONG addr=htodw(bp),result;
	result=trc_set_bp(m_sesId, TRC_HWBP_EXECUTE,NULL,(PVOID)addr, NULL);
	return result;
}

ULONG Debugger::set_bp(char * bp)
{
	ULONG addr=htodw(bp),result;
	result=trc_set_bp(m_sesId, TRC_BP_SOFTWARE,NULL,(PVOID)addr, NULL);
	return result;
}

ULONG Debugger::del_bp(char * bp)
{
	ULONG addr = htodw(bp),result;
	result = trc_delete_bp(m_sesId,NULL,(PVOID)addr, NULL);
	return result;
}

int Debugger::load_target( char *target, ULONG options )
{
	printf("loading %s\n", target);
		if(!trc_load_target(m_sesId, target, options ))//| TRC_OPT_BREAK_ON_MOD_EP  
		{
			printf("can\'t load target!!\n");
			return 0;
		}
	return 1;
}

void Debugger::print_mem(char *mem_addr)
{
	ULONG	addr=htodw(mem_addr);

	int i=0,k=0,align=16,padding=0,pktSize=128;
	u_char buff[128];
	u_char *p=buff;
	trc_read_memory(m_sesId,(PVOID)addr,buff,128);
		while(pktSize)
		{
			if(pktSize<16)
			{
				align=pktSize;
				padding=16-pktSize;
			}
			k=i;
			for(int j=0;j<align;j++,k++)
			{
				printf("%.2x ",p[k]);
			}
			
			for(int t=0;t<padding;t++)
			{
				printf("   ");
			}
			
			printf("\t");
			k=i;
			for(int j=0;j<align;j++,k++){
				if(p[k]==0x0a||p[k]==0x0d||p[k]==0x09){
					printf(".");
				} else {
					printf("%c",p[k]);
				}
			}
		i+=align;
					
			printf("\n");
			pktSize-=align;
				
		}
}

void Debugger::print_regs(ULONG tid)
{
	CONTEXT ctx;
	ZeroMemory(&ctx,sizeof(ctx));
	if(trc_get_thread_ctx(m_sesId,tid,&ctx))
	{
		printf("eax=%.8x ebx=%.8x ecx=%.8x edx=%.8x edi=%.8x esi=%.8x\nebp=%.8x esp=%.8x eip=%.8x\n",
			ctx.Eax,ctx.Ebx,ctx.Ecx,ctx.Edx,ctx.Edi,ctx.Esi,ctx.Ebp,ctx.Esp,ctx.Eip);
	}
}

ULONG Debugger::split_cmd(char * str,char **cmd,char **arg)
{
	int i=0,j=0,retval=0;
	PCHAR p=str;
	*cmd=p;
	
	while(TRUE)
	{
		if(*p==0x20)
		{
			retval=1;
			break;
		}
		
		if(*p==0x0)
		{
			retval=0;
			break;
		}
		p++;
	}

	*p=0x0;
	p++;
	*arg=p;
	
	return retval;
}

ULONG __stdcall Debugger::excpt_event(PTRC_EXCEPTION_EVENT evt)
{
	printf("event code: %x,EIP=%x\n",evt->EventCode,evt->Frame.Eip);
	return TRC_EXC_NOT_HANDLE;
}

ULONG __stdcall Debugger::dbg_event(PTRC_EXCEPTION_EVENT evt)
{
	CONTEXT	ctx;
	ULONG	retValue=NULL;
	
	printf("***********\nexception in thread %x thread teb id %x\n",evt->CurrentThread->TID,evt->CurrentThread->teb_addr);
	
	//эти сообщения получает TRC_DBG_EVENTS_CALLBACK
	if(evt->EventCode==TRC_EVENT_BREAKPOINT)
	{
		if(evt->CurrentModule)
			printf("mod name %s\n**********",evt->CurrentModule->ModName);
	
		printf("breakpoint at %x\n",evt->Frame.Eip);
		print_regs((ULONG)evt->CurrentThread->TID);
		PMnemonics mnem=DasmDisasmCmd(m_sesId,(PVOID)evt->Frame.Eip,0x30);
		if(mnem)
		{
		//	//for(int i=0;i<mnem->count;i++)
		//	//{
		//	//	printf("%s\n",mnem->row[i].mnem.AsmCode);
		//	//}
			DasmFreeBuffer(mnem);
		}

		m_critsec.Enter();
		retValue = m_trace_mode;
		m_critsec.Leave();
	}
	else if(evt->EventCode==TRC_EVENT_SINGLE_STEP)
	{
		if(evt->CurrentModule)
			printf("mod name %s\n**********",evt->CurrentModule->ModName);

		DWORD b=0;
		printf("single step at address: %x\n",evt->Frame.Eip);
		_asm int 3;
		print_regs((ULONG)evt->CurrentThread->TID);
		PMnemonics mnem=DasmDisasmCmd(m_sesId,(PVOID)evt->Frame.Eip,0x30);
		if(mnem){
			for(int i=0;i<mnem->count;i++){
			printf("%s\n",mnem->row[i].mnem.AsmCode);
			}
		DasmFreeBuffer(mnem);
		}
		m_critsec.Enter();
		retValue = m_trace_mode;
		m_critsec.Leave();
	} 
	
	return retValue;
}

ULONG __stdcall Debugger::proc_event(PTRC_PROCESS_EVENT evt)
{

	if(evt->EventCode==TRC_EVENT_TERMINATE)
	{
		printf("process terminated\n");
	}
	else if(evt->EventCode==TRC_EVENT_THREAD_START)
	{
		printf("new thread started thread id is %x thread teb is %x\n",evt->StartedThread->TID,evt->StartedThread->teb_addr);
	}
	else if(evt->EventCode==TRC_EVENT_THREAD_EXIT)
	{
		printf("thread exited thread id is %x exit code is %x\n",evt->ExitedThread->TID,evt->ExitedThread->exit_code);
	}
	else if(evt->EventCode==TRC_EVENT_LOAD_MODULE)
	{
		printf("module %s loaded,image Base is %#x size is %#x\n",evt->LoadedModule->ModName,evt->LoadedModule->ImageBase,evt->LoadedModule->ImageSize);
	}
	else if(evt->EventCode==TRC_EVENT_UNLOAD_MODULE)
	{
		printf("module %s unloaded,image Base is %#x size is %#x\n",evt->UnloadedModule->ModName,evt->UnloadedModule->ImageBase,evt->UnloadedModule->ImageSize);
	}
	return 0;
}

void Debugger::load( const wxString &path, ULONG options )
{
	
	load_target(const_cast<char*>(path.c_str()), options);
}

void Debugger::unload()
{
	trc_session_close(m_sesId);
}

void Debugger::open_session( ULONG options, LPVOID params )
{
	// TRC_SESSION_LOCAL, NULL
	m_sesId = trc_session_open(options, params);
}

void Debugger::close_session()
{
	trc_session_close(m_sesId);
	m_sesId = 0;
}


/*
ULONG __stdcall trc_event2(PTRC_EVENT evt, void *arg)
{
	switch(evt->EventCode){
		case TRC_EVENT_BREAKPOINT:
			printf("bp at addr:%x\n",evt->Frame.Eip);
			break;
	}
	return 0;
}
*/
