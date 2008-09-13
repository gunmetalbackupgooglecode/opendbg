/*
    *    
    * Copyright (c) 2008 
    * Hobleen
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

#include "trc_types.h"


void set_tf_user(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent);
void set_tf_shadow(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent);

//#define QUICK_CONTEXT



PMODULE_LIST __cdecl get_module_by_addr(PSESSION_INFO pSessData, UINT_PTR addr)
{
	char sz[256];
	PMODULE_LIST pCurModule; // esi@1
	UINT_PTR need_addr; // ebx@2
	UINT_PTR mod_end; // ebp@3
	
	//PMODULE_LIST pFirstMod; // [sp+10h] [bp-204h]@1

	pCurModule = pSessData->pModules;	
	if ( !pCurModule )
		return 0;

	OutputDebugString("searching modules\n");
	need_addr = addr;
	//wsprintf(OutputString, "addr is %x\n", addr);
	//OutputDebugString(OutputString);
	while (pCurModule)
	{
		mod_end = (UINT_PTR)pCurModule->data.ImageBase + pCurModule->data.ImageSize;
		wsprintf(sz, "  base is %x img size is %x\n", pCurModule->data.ImageBase, pCurModule->data.ImageSize);
		//OutputDebugString(sz);
		if ( (UINT_PTR)pCurModule->data.ImageBase <= need_addr )
		{
			if ( need_addr < mod_end )
			{
				OutputDebugString(pCurModule->data.ModName);
				OutputDebugString("\n");
				return pCurModule;				
			}
		}
		pCurModule = pCurModule->pNext;		
	}	
	return 0;	
}


ULONG __cdecl DbgEv2ExcEv(PDBG_EVENT pDbgEvent, PTRC_EXCEPTION_EVENT a2)
{
//	ULONG result; // eax@1
	PDBG_EVENT pEvent; // eax@1

	pEvent = pDbgEvent;	
	a2->Frame.Dr0 = pDbgEvent->exception.TrapFrame.Dr0; // DR0?
	a2->Frame.Dr1 = pDbgEvent->exception.TrapFrame.Dr1;	// DR1
	a2->Frame.Dr2 = pDbgEvent->exception.TrapFrame.Dr2;	// DR2
	a2->Frame.Dr3 = pDbgEvent->exception.TrapFrame.Dr3;	// DR3
	a2->Frame.Dr6 = pDbgEvent->exception.TrapFrame.Dr6;	// DR6
	a2->Frame.Dr7 = pDbgEvent->exception.TrapFrame.Dr7;	// DR7
	a2->Frame.SegGs = pDbgEvent->exception.TrapFrame.SegGs;;
	a2->Frame.SegEs = pDbgEvent->exception.TrapFrame.SegEs;	// SegEs
	a2->Frame.SegDs = pDbgEvent->exception.TrapFrame.SegDs;	// SegDs
	a2->Frame.SegFs = pDbgEvent->exception.TrapFrame.SegFs;	// SegFs
	a2->Frame.SegCs = pDbgEvent->exception.TrapFrame.SegCs;	// SegCs
	a2->Frame.SegSs = pDbgEvent->exception.TrapFrame.HardwareSegSs;	// SegSs

	a2->Frame.Edx = pDbgEvent->exception.TrapFrame.Edx;	// Edx
	a2->Frame.Ecx = pDbgEvent->exception.TrapFrame.Ecx;	// Ecx
	a2->Frame.Eax = pDbgEvent->exception.TrapFrame.Eax;	// Eax	
	a2->Frame.Ebx = pDbgEvent->exception.TrapFrame.Ebx;	// Ebx
	a2->Frame.Ebp = pDbgEvent->exception.TrapFrame.Ebp;	// Ebp

	a2->Frame.Edi = pDbgEvent->exception.TrapFrame.Edi;	// Edi
	a2->Frame.Esi = pDbgEvent->exception.TrapFrame.Esi;	// Esi
	a2->Frame.Eip = pDbgEvent->exception.TrapFrame.Eip;	// Eip
	a2->Frame.Esp = pDbgEvent->exception.TrapFrame.HardwareEsp;	// Esp

	
	a2->Frame.EFlags = pDbgEvent->exception.TrapFrame.EFlags;	// EFlags
	return 0;
}


ULONG __cdecl ExcEv2DbgEv(PDBG_EVENT pDbgEvent, PTRC_EXCEPTION_EVENT a2)
{
//	ULONG result; // eax@1
	PDBG_EVENT pEvent; // eax@1

	pEvent = pDbgEvent;	
	pDbgEvent->exception.TrapFrame.Dr0 = a2->Frame.Dr0; // DR0?
	pDbgEvent->exception.TrapFrame.Dr1 = a2->Frame.Dr1;	// DR1
	pDbgEvent->exception.TrapFrame.Dr2 = a2->Frame.Dr2;	// DR2
	pDbgEvent->exception.TrapFrame.Dr3 = a2->Frame.Dr3;	// DR3
	pDbgEvent->exception.TrapFrame.Dr6 = a2->Frame.Dr6;	// DR6
	pDbgEvent->exception.TrapFrame.Dr7 = a2->Frame.Dr7;	// DR7

	pDbgEvent->exception.TrapFrame.SegGs = a2->Frame.SegGs;;
	pDbgEvent->exception.TrapFrame.SegEs = a2->Frame.SegEs;	// SegEs
	pDbgEvent->exception.TrapFrame.SegDs = a2->Frame.SegDs;	// SegDs
	pDbgEvent->exception.TrapFrame.SegFs = a2->Frame.SegFs;	// SegFs
	pDbgEvent->exception.TrapFrame.SegCs = a2->Frame.SegCs;	// SegCs
	pDbgEvent->exception.TrapFrame.HardwareSegSs = a2->Frame.SegSs;	// SegSs

	pDbgEvent->exception.TrapFrame.Edx = a2->Frame.Edx;	// Edx
	pDbgEvent->exception.TrapFrame.Ecx = a2->Frame.Ecx;	// Ecx
	pDbgEvent->exception.TrapFrame.Eax = a2->Frame.Eax;	// Eax	
	pDbgEvent->exception.TrapFrame.Ebx = a2->Frame.Ebx;	// Ebx
	pDbgEvent->exception.TrapFrame.Ebp = a2->Frame.Ebp;	// Ebp

	pDbgEvent->exception.TrapFrame.Esi = a2->Frame.Esi;	// Esi
	pDbgEvent->exception.TrapFrame.Edi = a2->Frame.Edi;	// Edi
	pDbgEvent->exception.TrapFrame.Eip = a2->Frame.Eip;	// Eip
	pDbgEvent->exception.TrapFrame.HardwareEsp = a2->Frame.Esp;	// Eip

	
	pDbgEvent->exception.TrapFrame.EFlags = a2->Frame.EFlags;	// EFlags
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//Exception handling - main part
//
//

ULONG __stdcall OnEventProc(PDBG_EVENT pDbg, ULONG SessId)
{
	PSESSION_INFO pSessData; // esi@1

	PTHREAD_DATA pCurThrd; // eax@6	
	PDBG_EVENT pDbgEvent; // edi@6	
	PTRC_PROC_EVT term_callback; // eax@11
	PTRC_PROC_EVT proc_callback; // eax@22
	PTHREAD_DATA exit_thrd; // eax@24	
	PMODULE_LIST new_mod; // eax@28	
	PVOID hDbg_file; // eax@30
	PVOID ep_addr; // ebp@31
	PMODULE_LIST unload_mod; // eax@36	
	PTRC_PROC_EVT unload_callback; // eax@37	
	PTHREAD_DATA_EX pNew_thrd; // eax@14	

	PVOID unload_mod_base; // eax@36
	TRC_EXCEPTION_EVENT ExceptEvent; // [sp+28h] [bp-718h]@6
	TRC_PROCESS_EVENT ProcEvent; // [sp+10h] [bp-730h]@6
	char pBuffer[1000]; // [sp+358h] [bp-3E8h]@31
	CONTEXT Context; // [sp+8Ch] [bp-6B4h]@42
	ULONG res;
	char sz[256];


	pSessData = get_sess_by_id(SessId);
	if (!pSessData)
		return RES_NOT_HANDLED;

	OutputDebugString("\n\tEvent arrived******************\n");
		
	memset(&ExceptEvent, 0, sizeof(ExceptEvent));
	memset(&ProcEvent, 0, sizeof(ProcEvent));
	memset(&Context, 0, sizeof(Context));
	Context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
	pDbgEvent = pDbg;	
	

	switch ( pDbgEvent->event_code )
	{
	case DBG_EXCEPTION:
		OutputDebugString("DBG_EXCEPTION\n");
		
		wsprintf(pBuffer, "Exception code = %X\n", pDbg->exception.ExceptionRecord.ExceptionCode);
		OutputDebugString(pBuffer);

		pCurThrd = get_thrd_by_tid(pSessData, pDbg->thread_id);		

		DbgEv2ExcEv(pDbgEvent, &ExceptEvent);
		
#ifndef  QUICK_CONTEXT
		CONTEXT Con;
		dbg_get_thread_ctx(pCurThrd->dbg_handle, &Con);
		Con2TrcFrame(&ExceptEvent.Frame, &Con);		
		
#endif

		pCurThrd->pFastContext = &ExceptEvent.Frame;
		res = ProcessException(pDbgEvent, pSessData, &ExceptEvent);

		
		
		if (pSessData->options & TRC_OPT_ONCE_BREAK)
		{
			PBREAKPOINT_LIST pBp;
			if (pBp = find_soft_bp_by_addr(pSessData->pSoftBps, (PVOID)ExceptEvent.Frame.Eip))
			{
				pSessData->CC_need_addr = (PVOID)ExceptEvent.Frame.Eip;
				dbg_write_memory(pSessData->pDbgContext, (PVOID)ExceptEvent.Frame.Eip, &pBp->SavedByte, 1, 1);
			}

			for (int i=0; i<4; i++)
			{
				if ((pCurThrd->breakpoint[i].bEnabled)
					&& (pCurThrd->breakpoint[i].addr == (PVOID)ExceptEvent.Frame.Eip)
					&& (pCurThrd->breakpoint[i].type == TRC_HWBP_EXECUTE))
				{
					pCurThrd->EnableDrAddr = (PVOID)ExceptEvent.Frame.Eip;
					trc_enable_bp(pSessData->SessionId, TRC_HWBP_EXECUTE, (PVOID)ExceptEvent.Frame.Eip, pCurThrd->trc_thrd.TID, FALSE);				
					set_tf_shadow(pSessData, &ExceptEvent);
					break;
				}
			}
		}
		
		wsprintf(pBuffer, "dr7=%X, dr0=%X\n", ExceptEvent.Frame.Dr7, ExceptEvent.Frame.Dr0);
		OutputDebugString(pBuffer);

		pCurThrd->pFastContext = 0;
		ExcEv2DbgEv(pDbgEvent, &ExceptEvent);		
//#ifndef  QUICK_CONTEXT		
//		TrcFrame2Con(&Con, &ExceptEvent.Frame);
//		dbg_set_thread_ctx(pCurThrd->dbg_handle, &Con);		
//#endif		
		
		//pCurThrd = get_thrd_list(pSessData, pDbg->thread_id);		

		/*
		if (pCurThrd->bNextTraceCmd)
		{
			OutputDebugString("bNextTraceCmd is set, bProgTrace\n");
			pCurThrd->bProgTrace = TRUE;
		}
		*/

		wsprintf(sz, "bDbg = %X, bProc = %X\n", pCurThrd->bDbgTraced, pCurThrd->bProgTrace);
		OutputDebugString(sz);
		/*
		dbg_read_memory(pSessData->pDbgContext, (PVOID)ExceptEvent.Frame.Eip, pBuffer, 20, TRUE);

		if ((UCHAR)pBuffer[0] == 0x9D)
		{
			pCurThrd->bNextTraceCmd = TRUE;
			OutputDebugString("bNextTraceCmd\n");
		}
		*/
#ifdef QUICK_CONTEXT
		return RES_CORRECT_FRAME | res;
#else
		return res;
#endif

	case DBG_TERMINATED:
		OutputDebugString("DBG_TERMINATED\n");
		ProcEvent.EventCode = TRC_EVENT_TERMINATE;
		term_callback = (PTRC_PROC_EVT)pSessData->ProcEvent.lpCallback;
		if ( term_callback )
			term_callback(&ProcEvent, pSessData->ProcEvent.arg);

		OutputDebugString("target terminated\n");
		pSessData->bTerminated = TRUE;
		return RES_CONTINUE;

	case DBG_START_THREAD:
		OutputDebugString("DBG_START_THREAD\n");
		ProcEvent.EventCode = TRC_EVENT_THREAD_START;
		pNew_thrd = add_thread(pSessData, &pDbgEvent->thread_start);		
		if ( !pNew_thrd )
			return RES_CONTINUE;

		OutputDebugString("thread added to table\n");
		if (pSessData->options & TRC_OPT_BREAK_ON_NEW_THREAD)
			set_bp_one_shot(pSessData, (PVOID)pDbgEvent->thread_start.initial_context.Eip);

		ProcEvent.StartedThread = &pNew_thrd->data.trc_thrd;
		proc_callback = (PTRC_PROC_EVT)pSessData->ProcEvent.lpCallback;
		if ( proc_callback )
			proc_callback(&ProcEvent, pSessData->ProcEvent.arg);
		return RES_CONTINUE;

	case DBG_EXIT_THREAD:
		OutputDebugString("DBG_EXIT_THREAD\n");

		ProcEvent.EventCode = TRC_EVENT_THREAD_EXIT;
		exit_thrd = get_thrd_by_tid(pSessData, (ULONG)pDbgEvent->thread_exit.thread_id);
		if ( exit_thrd )
		{
			exit_thrd->trc_thrd.exit_code = pDbgEvent->thread_exit.exit_code;
			ProcEvent.ExitedThread =  &exit_thrd->trc_thrd;
		}

		proc_callback = (PTRC_PROC_EVT)pSessData->ProcEvent.lpCallback;
		if ( proc_callback )
			proc_callback(&ProcEvent, pSessData->ProcEvent.arg);

		delete_thread(pSessData, &pDbgEvent->thread_exit);
		return RES_CONTINUE;

	case DBG_LOAD_DLL:
		OutputDebugString("DBG_LOAD_DLL\n");
		ProcEvent.EventCode = TRC_EVENT_LOAD_MODULE;
		new_mod = add_module(pSessData, &pDbgEvent->dll_load);		
		if ( !new_mod )
			return 0;
		//OutputDebugString("Mod added ok!");
		
		if ( (pSessData->options & TRC_OPT_BREAK_ON_MOD_EP) && (UINT_PTR)pSessData->pDbgContext)
		{
			wsprintf(pBuffer, "dbg_open_file(%X, %s, %X, %X)", pSessData->pDbgContext, &pDbgEvent->dll_load.file_name, GENERIC_READ, OPEN_EXISTING);
			//OutputDebugString(pBuffer);

			hDbg_file = dbg_open_file(pSessData->pDbgContext, pDbgEvent->dll_load.file_name, GENERIC_READ, OPEN_EXISTING); // 80000000

			if ( hDbg_file )
			{
				dbg_read_file(hDbg_file, &pBuffer, 1000);
				ep_addr = (PVOID)(get_ep_rva(&pBuffer) + (UINT_PTR)pDbgEvent->dll_load.image_base);

				OutputDebugString("trc_set_bp on mod ep\n");
				
				set_bp_one_shot(pSessData, ep_addr);
				
				dbg_close_file(hDbg_file);
				OutputDebugString("Bp set on module EP.\n");
			} else
				OutputDebugString("dbg_open_file returned 0\n");
			
		}
		
		ProcEvent.LoadedModule  = &new_mod->data;
		//LABEL_22:
		OutputDebugString("Callback calling...\n");
		proc_callback = (PTRC_PROC_EVT)pSessData->ProcEvent.lpCallback;
		if ( proc_callback )
			proc_callback(&ProcEvent, pSessData->ProcEvent.arg);
		OutputDebugString("Callback returned.\n");
		return RES_CONTINUE;

	case DBG_UNLOAD_DLL:
		OutputDebugString("DBG_UNLOAD_DLL\n");
		unload_mod_base = pDbgEvent->dll_unload.image_base;

		ProcEvent.EventCode = TRC_EVENT_UNLOAD_MODULE;
		unload_mod = get_module_by_base(pSessData, unload_mod_base);
		if ( !unload_mod )
			return 0;

		ProcEvent.UnloadedModule = &unload_mod->data;
		unload_callback = (PTRC_PROC_EVT)pSessData->ProcEvent.lpCallback;
		if ( unload_callback )
			unload_callback(&ProcEvent, pSessData->ProcEvent.arg);
		delete_module(pSessData, &pDbgEvent->dll_unload);
		return RES_CONTINUE;

	case DBG_RDTSC:
		OutputDebugString("DBG_RDTSC\n");
		wsprintf(pBuffer, "At addr = %X\n", pDbgEvent->rdtsc.addr);
		OutputDebugString(pBuffer);
	
		pDbgEvent->rdtsc.eax = pDbgEvent->rdtsc.edx = 0;
		return RES_CONTINUE;

	case DBG_CONTINUE_CALL:
		OutputDebugString("DBG_CONTINUE_CALL\n");
		pCurThrd = get_thrd_by_tid(pSessData, (ULONG)pDbg->thread_id);	
		OutputDebugString("1\n");
		if ( !pCurThrd ) // 85
			return RES_CONTINUE;
		OutputDebugString("2\n");

		OutputDebugString(" continue...\n");
		
		CONTEXT *con;
		con = &pDbg->seh_continue.context;
		if (pSessData->options & TRC_OPT_PROTECT_DRS)
		{
			
			pCurThrd->DrProc.dr0 = con->Dr0;
			pCurThrd->DrProc.dr1 = con->Dr1;
			pCurThrd->DrProc.dr2 = con->Dr2;
			pCurThrd->DrProc.dr3 = con->Dr3;		
			pCurThrd->DrProc.dr7 = con->Dr7;

			con->Dr0 = pCurThrd->DrDbg.dr0;
			con->Dr1 = pCurThrd->DrDbg.dr1;
			con->Dr2 = pCurThrd->DrDbg.dr2;
			con->Dr3 = pCurThrd->DrDbg.dr3;		
			con->Dr7 = pCurThrd->DrDbg.dr7;
		}
		PEXCEP_FRAME pCurFrame;
		pCurFrame = pCurThrd->pExcFrames;
		
		wsprintf(pBuffer, "EFlags = %X\n", con->EFlags);
		OutputDebugString(pBuffer);

		pCurThrd->bProgTrace = ((con->EFlags & 0x100) != 0);
		pCurThrd->bNextTraceCmd = 0;

		//wsprintf(pBuffer, "bProg = %X\n, bDbg = %X, CallRes = %X\n", pCurThrd->bProgTrace, pCurThrd->bDbgTraced, );
		//OutputDebugString(pBuffer);
		

		OutputDebugString("3\n");
		if (pCurFrame)
		{
			OutputDebugString("3.5\n");
			if ((pCurFrame->bDebugTraced) && (pSessData->options & TRC_OPT_CONTINUE_TRACE_AFTER_SEH))
			{
				OutputDebugString("Continue dbg trace after seh\n");
				pCurThrd->bDbgTraced = 1;
				//pCurThrd->LastCallbackRet = pCurFrame->CallRes;
				pCurThrd->TraceMode = pCurFrame->TraceMode;
				con->EFlags |= 0x100;
			} else {
				OutputDebugString("No dbg trace after seh\n");
				pCurThrd->bDbgTraced = 0;
			}
			OutputDebugString("4\n");
			
			for (ULONG i=0; i<pCurFrame->pSehs->count; i++)
				delete_bp(pSessData, (PVOID)pCurFrame->pSehs->seh[i].handler, TRC_BP_THREAD, pDbg->thread_id);
				
			pCurThrd->pExcFrames = pCurFrame->pPrev;
			FREE(pCurFrame->pSehs);
			FREE(pCurFrame);

			pCurFrame = pCurThrd->pExcFrames;
			if ((pCurFrame) && (pSessData->options & TRC_OPT_BREAK_ON_SEH_HANDLER))
			{
				OutputDebugString("TRC_OPT_BREAK_ON_SEH_HANDLER set\n");
				for (ULONG i=0; i<pCurFrame->pSehs->count; i++)
					set_bp_ex(pSessData, (PVOID)pCurFrame->pSehs->seh[i].handler, TRC_BP_THREAD, pDbg->thread_id, TRC_EVENT_BREAKPOINT);
					//set_bp_tid(pSessData, (PVOID)pCurFrame->pSehs->seh[i].handler, pDbg->thread_id);
			}			
		}

		OutputDebugString(" all ok!\n");
	default:
		

		return RES_CONTINUE | RES_CORRECT_FRAME;
	}
	return RES_CONTINUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//																		//
//				Exceptions and debug events handling					//
//																		//
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
ULONG process_step_into(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent);
ULONG process_step_over(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent);
ULONG process_trace_till_retn(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent);

//VOID set_step_into(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent);
VOID set_step_over(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent);
VOID set_trace_till_retn(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent);
//VOID set_step_into(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent);

ULONG process_callback(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExceptEvent)
{
	ULONG res;
	char sz[256];
	PTRC_EXCPT_EVT callback = (PTRC_EXCPT_EVT)pSess->DbgEvent.lpCallback;

	OutputDebugString("======Enter callback======\n");
	wsprintf(sz, "callback = %X\n", callback);
	OutputDebugString(sz);
	res = (callback) ? (callback(pExceptEvent, pSess->DbgEvent.arg)) : TRC_RUN;		
	OutputDebugString("======Leave callback======\n");
	wsprintf(sz, "res = %X\n", res);
	OutputDebugString(sz);
	if ((res & (res - 1)) || !res)
		res = TRC_RUN;



	return res;
}

void process_callback_nil(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExceptEvent)
{
	/*
	PTHREAD_DATA pThrd = get_thrd_list(pSess, pExceptEvent->CurrentThread->TID);
	pThrd->bProgTrace = pExceptEvent->Frame.EFlags & 0x100;

	char sz[256];
	wsprintf(sz, "Proc Trace = %X\n", pThrd->bProgTrace);
	OutputDebugString(sz);
	*/
}



VOID process_call_res(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent, ULONG call_res)
{
	char sz[256];
	wsprintf(sz, "process_call_res(%X, %X, %X, %X)", pSess, pExcepEvent, pDbgEvent, call_res);
	OutputDebugString(sz);

	switch (call_res) {
	case TRC_STEALTH_TRACE:
		OutputDebugString("Process shadow by step_into\n");
	case TRC_STEP_INTO:
		OutputDebugString("TRC_STEP_INTO set...\n");
//		set_step_into(pSess, pExcepEvent, pDbgEvent);
		set_tf_user(pSess, pExcepEvent);
		break;
	case TRC_STEP_OVER:
		set_step_over(pSess, pExcepEvent, pDbgEvent);
		break;
	case TRC_RUN_TIL_RET:
		set_trace_till_retn(pSess, pExcepEvent, pDbgEvent);
		break;
	}

	//PTHREAD_DATA pThrd = get_thrd_by_tid(pSess, pDbgEvent->thread_id);
	//wsprintf(sz, "bProc = %X, bDbg = %X\n", pThrd->bProgTrace, pThrd->bDbgTraced);
	//OutputDebugString(sz);
}

//////////////////////////////////////////////////////////////////////////
//if callback return XXX then call set_XXX
//set TF, bp, etc.

void set_tf_shadow(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent)
{
	UINT_PTR over_step_addr;
	UCHAR buffer[16];
	trc_read_memory(pSess->SessionId, (PVOID)pExcepEvent->Frame.Eip, buffer, sizeof(buffer));

	OutputDebugString("======Set Step Into======\n");
	if (IsUntraceableCmd(buffer) && (!IsChainCmd(buffer)))
	{
		over_step_addr = pExcepEvent->Frame.Eip + Get_code_size(pSess, (PVOID)pExcepEvent->Frame.Eip);
		set_bp_ex(pSess, (PVOID)over_step_addr, TRC_BP_SHADOW, (ULONG)pExcepEvent->CurrentThread->TID, TRC_EVENT_SINGLE_STEP);				
	} else
	{
		pExcepEvent->Frame.EFlags |= 0x100;			
		PTHREAD_DATA pThrd = get_thrd_by_tid(pSess, pExcepEvent->CurrentThread->TID);
		pThrd->bDbgTraced = TRUE;
		if (!pThrd->TraceMode)
			pThrd->TraceMode = TRC_BP_SHADOW;
	}
}


void set_tf_till_ret(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent)
{
	UINT_PTR over_step_addr;
	UCHAR buffer[16];
	trc_read_memory(pSess->SessionId, (PVOID)pExcepEvent->Frame.Eip, buffer, sizeof(buffer));

	OutputDebugString("======Set Step Into======\n");
	if (IsUntraceableCmd(buffer))
	{
		over_step_addr = pExcepEvent->Frame.Eip + Get_code_size(pSess, (PVOID)pExcepEvent->Frame.Eip);
		set_bp_ex(pSess, (PVOID)over_step_addr, TRC_BP_TIL_RET, (ULONG)pExcepEvent->CurrentThread->TID, TRC_EVENT_SINGLE_STEP);				
	} else
	{
		pExcepEvent->Frame.EFlags |= 0x100;			
		PTHREAD_DATA pThrd = get_thrd_by_tid(pSess, pExcepEvent->CurrentThread->TID);
		pThrd->bDbgTraced = TRUE;
		if (pThrd->TraceMode != TRC_BP_USER)
			pThrd->TraceMode = TRC_BP_TIL_RET;
	}
}

void set_tf_user(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent)
{
	UINT_PTR over_step_addr;
	UCHAR buffer[16];
	trc_read_memory(pSess->SessionId, (PVOID)pExcepEvent->Frame.Eip, buffer, sizeof(buffer));

	OutputDebugString("======Set Step Into======\n");
	if (IsUntraceableCmd(buffer))
	{
		over_step_addr = pExcepEvent->Frame.Eip + Get_code_size(pSess, (PVOID)pExcepEvent->Frame.Eip);
		set_bp_ex(pSess, (PVOID)over_step_addr, TRC_BP_THREAD, (ULONG)pExcepEvent->CurrentThread->TID, TRC_EVENT_SINGLE_STEP);				
	} else
	{
		OutputDebugString("Setting TF for trace...");
		pExcepEvent->Frame.EFlags |= 0x100;			
		PTHREAD_DATA pThrd = get_thrd_by_tid(pSess, pExcepEvent->CurrentThread->TID);
		pThrd->bDbgTraced = TRUE;
		pThrd->TraceMode = TRC_BP_USER;
	}
}

/*VOID set_step_into(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent)
{
	set_tf_user(pSess, pExcepEvent);
}
*/


VOID set_step_over(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent)
{
	UINT_PTR over_step_addr, except_addr;
	except_addr = (UINT_PTR)pDbgEvent->exception.ExceptionRecord.ExceptionAddress;
	

	if (IsTraceOverCmd(pSess, (PVOID)except_addr))
	{					
		over_step_addr = except_addr + Get_code_size(pSess, (PVOID)except_addr);		
		set_bp_ex(pSess, (PVOID)over_step_addr, TRC_BP_THREAD, (ULONG)pDbgEvent->thread_id, TRC_EVENT_SINGLE_STEP);
	}
	else
	{
		set_tf_user(pSess, pExcepEvent);
	}
}

VOID set_trace_till_retn(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent)
{
	UINT_PTR except_addr = (UINT_PTR)pDbgEvent->exception.ExceptionRecord.ExceptionAddress;
	UCHAR buff[16];

	OutputDebugString("======Set Trace Til Ret======\n");
	trc_read_memory(pSess->SessionId, (PVOID)except_addr, &buff, 16);


	UINT_PTR over_step_addr;
	//except_addr = (ULONG)pDbgEvent->exception.ExceptionRecord.ExceptionAddress;


	if (IsTraceOverCmd(pSess, (PVOID)except_addr))
	{					
		over_step_addr = except_addr + Get_code_size(pSess, (PVOID)except_addr);		
		set_bp_ex(pSess, (PVOID)over_step_addr, TRC_BP_TIL_RET, (ULONG)pDbgEvent->thread_id, TRC_EVENT_SINGLE_STEP);
	}
	else
	{
		set_tf_till_ret(pSess, pExcepEvent);
	}
}

//////////////////////////////////////////////////////////////////////////
//if RetCode is XXX then call process_XXX
//return callback return cmd - trace, run, etc.


ULONG process_trace_till_retn(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent)
{
	OutputDebugString("---process_trace_till_retn---\n");
	UCHAR buff[16];
	PVOID except_addr = pDbgEvent->exception.ExceptionRecord.ExceptionAddress;
	trc_read_memory(pSess->SessionId, (PVOID)except_addr, &buff, 16);

	if ((buff[0] & 0xF6) == 0xC2) // it is retn
	{
		return process_callback(pSess, pExcepEvent);
	}
	else //not retn
	{
		process_callback_nil(pSess, pExcepEvent);
		//set_trace_till_retn(pSess, pExcepEvent, pDbgEvent);
		return TRC_RUN_TIL_RET;
	}
}

ULONG process_bp(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent)
{
//	PBREAKPOINT_LIST pBp_2;
	PTHREAD_DATA pThreadData;
	PVOID except_addr;
	ULONG res;
	static BYTE byCC = 0xCC;
//	char sz[256];

	except_addr = pDbgEvent->exception.ExceptionRecord.ExceptionAddress; // ExceptionAddress
	pThreadData = get_thrd_by_tid(pSess, (ULONG)pDbgEvent->thread_id);

	switch (process_bp_break(pSess, (PVOID)except_addr, pDbgEvent->thread_id, &pExcepEvent->EventCode))
	{
	case TRC_BP_USER:		
		OutputDebugString("Really TRC_BP_USER\n");
		res = process_callback(pSess, pExcepEvent);
		break;
	case TRC_BP_TIL_RET:
		OutputDebugString("Really TRC_BP_TIL_RET\n");
		res = process_trace_till_retn(pSess, pExcepEvent, pDbgEvent);
		break;
	default:
		OutputDebugString("Really Nothing!\n");
		res = TRC_RUN;
	}

	PBREAKPOINT_LIST pBp;
	if (pBp = find_soft_bp_by_addr(pSess->pSoftBps, (PVOID)except_addr))
	{
		OutputDebugString("Delete bp CC for a moment\n");
		dbg_write_memory(pSess->pDbgContext, (PVOID)except_addr, &pBp->SavedByte, sizeof(BYTE), 1);
		pSess->CC_need_addr = except_addr;
		set_tf_shadow(pSess, pExcepEvent);
	}
	return res;
}

//----- (100036C0) --------------------------------------------------------
signed long __cdecl ProcessException(PDBG_EVENT pDbgEvent, PSESSION_INFO pSessData, PTRC_EXCEPTION_EVENT pExcepEvent)
{
	ULONG SessId; // esi@1
	signed long except_code; // eax@4
	PBREAKPOINT_LIST pBpBreaked; // eax@10	
	ULONG except_callback_res; // eax@84	
	PMODULE_LIST pModule; // eax@2		
	PTHREAD_DATA pThreadData; // [sp+1Ch] [bp-3FCh]@1
	unsigned long temp; // [sp+24h] [bp-3F4h]@1	
	PVOID except_addr; // [sp+18h] [bp-400h]@4	
	UCHAR Buffer16[16]; // [sp+28h] [bp-3F0h]@26	
	CHAR OutputString[256]; // [sp+314h] [bp-104h]@44	
	EXCEPTION_REGISTRATION ExcepFrame;
	ULONG call_res;

	SessId = pSessData->SessionId;
	except_addr = (PVOID)pDbgEvent->exception.ExceptionRecord.ExceptionAddress; // ExceptionAddress?
	except_code = pDbgEvent->exception.ExceptionRecord.ExceptionCode; // ExceptionCode


	pThreadData = get_thrd_by_tid(pSessData, (ULONG)pDbgEvent->thread_id);
	//if (pThreadData)
	//	pThreadData->pFastContext = &pExcepEvent->Frame;	

	pModule = (except_addr) ? get_module_by_addr(pSessData, (UINT_PTR)except_addr) : NULL;
	
	pExcepEvent->EventCode = pDbgEvent->exception.ExceptionRecord.ExceptionCode;
	pExcepEvent->CurrentProcess = &pSessData->trc_process;
	pExcepEvent->CurrentModule = (pModule) ? &pModule->data : NULL;	//	dw_60
	pExcepEvent->CurrentThread = &pThreadData->trc_thrd;	

	wsprintf(OutputString, "ProcessException: addr=%X, code=%X\n", except_addr, except_code);
	OutputDebugString(OutputString);
	wsprintf(OutputString, "pModule = %X, CurrentModule = %X\n", pModule, pExcepEvent->CurrentModule);
	OutputDebugString(OutputString);

	

	//CONTEXT con;		
	//con.ContextFlags = CONTEXT_FULL;
	//dbg_get_thread_ctx(pThreadData->dbg_handle, &con);
	wsprintf(OutputString, "EFLAGS = %X\n", pExcepEvent->Frame.EFlags);
	OutputDebugString(OutputString);


	if ( except_code == EXCEPTION_BREAKPOINT ) // 0x80000003
	{
		pBpBreaked = find_soft_bp_by_addr(pSessData->pSoftBps, (PVOID)except_addr);
		if (!pBpBreaked)
			goto l_exep;		
		dbg_read_memory(pSessData->pDbgContext, (PVOID)except_addr, &Buffer16, 1, 1);
		if (Buffer16[0] != 0xCC)
			goto l_exep;

		bool bRaiseEx = (pBpBreaked->SavedByte == 0xCC);


		OutputDebugString("EXCEPTION_BREAKPOINT\n");
		pDbgEvent->exception.TrapFrame.Eip--; // Eip
		pExcepEvent->Frame.Eip--; // 4C	
		restore_bp(pSessData, pThreadData, (PVOID)pExcepEvent->Frame.Eip);
		
		call_res = process_bp(pSessData, pExcepEvent, pDbgEvent);

		//pThreadData->LastCallbackRet = call_res;
		if (!pThreadData->bDbgTraced)
			pThreadData->bProgTrace = ((pExcepEvent->Frame.EFlags & 0x100) != 0);

		process_call_res(pSessData, pExcepEvent, pDbgEvent, call_res);		

		if (bRaiseEx)
			goto l_exep;

		//pThreadData->pFastContext = NULL;
		return RES_CONTINUE;
	}
	else if ( except_code == EXCEPTION_SINGLE_STEP ) // 0x80000004
	{
		if (pThreadData->bNeedCopyDr)
		{
			pExcepEvent->Frame.Dr0 = pThreadData->DrDbg.dr0;
			pExcepEvent->Frame.Dr1 = pThreadData->DrDbg.dr1;
			pExcepEvent->Frame.Dr2 = pThreadData->DrDbg.dr2;
			pExcepEvent->Frame.Dr3 = pThreadData->DrDbg.dr3;		
			pExcepEvent->Frame.Dr7 = pThreadData->DrDbg.dr7;

			pThreadData->bNeedCopyDr = FALSE;
		}
		
		OutputDebugString("EXCEPTION_SINGLE_STEP\n");	
		restore_bp(pSessData, pThreadData, (PVOID)pExcepEvent->Frame.Eip);

		ExcepFrame.prev = (PEXCEPTION_REGISTRATION)(pExcepEvent->Frame.Dr6 & 0xF);
		if ( (pExcepEvent->Frame.Dr6 & 0xF) == 0 )	//	not HW DR0-3 Break; so, we are traced
		{
			if (!pThreadData->bDbgTraced) // it is not our trace step => by program
			{
				goto l_exep;		
			}
			pThreadData->bDbgTraced = 0;

			ULONG bProcTF = pThreadData->bProgTrace;// || (pExcepEvent->Frame.EFlags & 0x100);
			pExcepEvent->EventCode = TRC_EVENT_SINGLE_STEP;
			temp = 0;
			
			switch(pThreadData->TraceMode) {			
			case TRC_BP_USER:
				call_res = process_callback(pSessData, pExcepEvent);
				//call_res = process_trace_till_retn(pSessData, pExcepEvent, pDbgEvent);
				break;
			case TRC_BP_TIL_RET:
				call_res = process_trace_till_retn(pSessData, pExcepEvent, pDbgEvent);
				break;
			default:
				call_res = TRC_RUN;				
			}
		
			pThreadData->bDbgTraced = 0;
			pThreadData->TraceMode = 0;
			pThreadData->bProgTrace = ((pExcepEvent->Frame.EFlags & 0x100) != 0);
			//pThreadData->LastCallbackRet = call_res;
			process_call_res(pSessData, pExcepEvent, pDbgEvent, call_res);	

			wsprintf(OutputString, "pThreadData->bProgTrace is %X\n", pThreadData->bProgTrace);
			OutputDebugString(OutputString);

			if (bProcTF)
				goto l_exep;
		}
		else // it is HW0-3 break
		{
			OutputDebugString("is hw bp\n");

			DWORD dr_num = 0;
			temp = (pExcepEvent->Frame.Dr6 & 0xF)-1;
			while (temp--)
				dr_num++;

			if (!pThreadData->breakpoint[dr_num].bEnabled)
			{
				OutputDebugString("--not our hr!\n");
				goto l_exep;
			}

			pExcepEvent->EventCode = TRC_EVENT_BREAKPOINT;
			call_res = process_callback(pSessData, pExcepEvent);
			//pThreadData->LastCallbackRet = call_res;
			process_call_res(pSessData, pExcepEvent, pDbgEvent, call_res);

			if ((pThreadData->breakpoint[dr_num].bEnabled) &&
				(pThreadData->breakpoint[dr_num].addr == except_addr) &&
				(pThreadData->breakpoint[dr_num].type == TRC_HWBP_EXECUTE))
			{
				pThreadData->EnableDrAddr = (PVOID)except_addr;
				trc_enable_bp(pSessData->SessionId, TRC_HWBP_EXECUTE, (PVOID)except_addr, pThreadData->trc_thrd.TID, FALSE);				
				set_tf_shadow(pSessData, pExcepEvent);
			}	
		}

		wsprintf(OutputString, "Trace mode = %X\n", pThreadData->TraceMode);
		OutputDebugString(OutputString);
		
		//pThreadData->pFastContext = NULL;
		return RES_CONTINUE;
	}	//	if (except_code == EXCEPTION_SINGLE_STEP_
	else
	{
l_exep:
		OutputDebugString("EXCEPTION\n");	
		restore_bp(pSessData, pThreadData, (PVOID)pExcepEvent->Frame.Eip);

		//delete old ones
		if (pThreadData->pExcFrames)
		{
			OutputDebugString("Delete old bps...\n");
			for (unsigned int i=0; i<pThreadData->pExcFrames->pSehs->count; i++)
			{
				wsprintf(OutputString, "Delete bp #%X\n", pThreadData->pExcFrames->pSehs->seh[i].handler);
				OutputDebugString(OutputString);
				//delete_bp_trace(pSessData, (PVOID)pThreadData->pExcFrames->pSehs->seh[i].handler, pDbgEvent->thread_id);
				delete_bp(pSessData, (PVOID)pThreadData->pExcFrames->pSehs->seh[i].handler, pDbgEvent->thread_id, TRC_BP_THREAD);
			}
		}

		if (pThreadData->bDbgTraced)
		{
			if (!pThreadData->bProgTrace)
			{
				pExcepEvent->Frame.EFlags &= 0x100;
			} else 
			{
				pExcepEvent->Frame.EFlags |= 0x100;
			}
		}
		PEXCEP_FRAME pFrame = create_except_frame(pSessData, pThreadData);
		

		if (pSessData->options & TRC_OPT_BREAK_ON_KI_USER_EX_DISP)
		{ // trace from KiUserExceptionDispatch
			pExcepEvent->Frame.EFlags |= 0x100;
			pThreadData->bDbgTraced = TRUE;
			pThreadData->TraceMode = TRC_BP_USER;
		} else { // delete trace
			pExcepEvent->Frame.EFlags &= ~0x100;
			pThreadData->bDbgTraced = FALSE;
		}
		pThreadData->bProgTrace = FALSE;		

		wsprintf(OutputString, "In SEH, bDbg = %X, CallRes = %X\n", pFrame->bDebugTraced, pFrame->CallRes);
		OutputDebugString(OutputString);

		trc_read_memory(SessId, (PVOID)except_addr, &Buffer16, 4);
		wsprintf(OutputString, "in eip: %X, code = %X", *(DWORD*)Buffer16, pDbgEvent->exception.ExceptionRecord.ExceptionCode);
		OutputDebugString(OutputString);

		if ( pSessData->ExcEvent.lpCallback )
		{
			pExcepEvent->EventCode = pDbgEvent->exception.ExceptionRecord.ExceptionCode;
			except_callback_res = ((PTRC_EXCPT_EVT)pSessData->ExcEvent.lpCallback)(pExcepEvent, pSessData->ExcEvent.arg);
			if ( except_callback_res & TRC_EXC_HANDLE )
			{
				//pThreadData->pFastContext = NULL;
				return RES_CONTINUE | RES_CORRECT_FRAME;
			}
		}

		if ( pSessData->options & TRC_OPT_BREAK_ON_SEH_HANDLER )
		{
			wsprintf(OutputString, "teb_addr=%x\n", pThreadData->trc_thrd.teb_addr);
			OutputDebugString(OutputString);

			for (ULONG i=0; i<pFrame->pSehs->count; i++)
			{
				wsprintf(OutputString, "Set bp #%x at %X\n", i, pFrame->pSehs->seh[i]);
				OutputDebugString(OutputString);
				set_bp_tid(pSessData, (PVOID)pFrame->pSehs->seh[i].handler, pExcepEvent->CurrentThread->TID);
			}

		}	//	if (l_pSessData->options & TRC_OPT_BREAK_ON_SEH_HANDLER)				

		if (pSessData->options & TRC_OPT_PROTECT_DRS)
		{
			pThreadData->DrDbg.dr0 = pDbgEvent->exception.TrapFrame.Dr0;
			pThreadData->DrDbg.dr1 = pDbgEvent->exception.TrapFrame.Dr1;
			pThreadData->DrDbg.dr2 = pDbgEvent->exception.TrapFrame.Dr2;
			pThreadData->DrDbg.dr3 = pDbgEvent->exception.TrapFrame.Dr3;
			pThreadData->DrDbg.dr7 = pDbgEvent->exception.TrapFrame.Dr7;

			pDbgEvent->exception.TrapFrame.Dr0 = pThreadData->DrProc.dr0;
			pDbgEvent->exception.TrapFrame.Dr1 = pThreadData->DrProc.dr1;
			pDbgEvent->exception.TrapFrame.Dr2 = pThreadData->DrProc.dr2;
			pDbgEvent->exception.TrapFrame.Dr3 = pThreadData->DrProc.dr3;
			pDbgEvent->exception.TrapFrame.Dr7 = pThreadData->DrProc.dr7;				

			/*
			pThreadData->bNeedCopyDr = TRUE;
			if (!pThreadData->bDbgTraced)
			{
				pExcepEvent->Frame.EFlags |= 0x100;
				pThreadData->bDbgTraced = TRUE;
				pThreadData->TraceMode = TRC_BP_SHADOW;
			}
			*/
		}


		//pThreadData->pFastContext = NULL;
		return RES_CORRECT_FRAME | RES_NOT_HANDLED;
	}
}
/**/
