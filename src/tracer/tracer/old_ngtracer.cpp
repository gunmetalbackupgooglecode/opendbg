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

#include "windows.h"
//#include "dbgapi.h"
//#include "ngtracerapi.h"
#include "trc_types.h"
#define NOT_PWD_2(x)		((x) & ((x)-1))

///////////////////////////////////////////////////////
// global variables

PSESSION_INFO_EX lpFirstSessListItem;
//SESSION_INFO* pCurSessData;

LPVOID hHeap;
BOOL by_is_inited;



//////////////////////////////////////////////////////////////////////////
// implementations


extern DWORD get_ep_rva(PVOID pModBase);
DWORD get_tls_dir(PVOID pModBase, ULONG num);

PSESSION_INFO get_sess_by_id(ULONG SessId)
{
	PSESSION_INFO_EX pCur;

	pCur = lpFirstSessListItem;
	while (pCur)
	{
		if (pCur->data.SessionId == SessId)
		{
			//OutputDebugString("sess found");
			return &pCur->data;
		}
		pCur = pCur->pNext;
	}
	return NULL;
}

TRACERAPI PPROCESS_INFO_EX trc_enum_processes(ULONG sesId)
{
	PSESSION_INFO pCurSessData = get_sess_by_id(sesId);

	if (pCurSessData && (pCurSessData->remote_id != TRC_SESSION_INVALID))
			return dbg_enum_processes((PVOID)pCurSessData->remote_id);	
	return 0;
}


TRACERAPI int trc_init()
{
	if (by_is_inited)
		return 1;

	hHeap = HeapCreate(0, 0x50000, 0);
	by_is_inited = 1;
	return dbg_initialize_api(0x75ECB86B, 0x0FB0B4647);
}


TRACERAPI int trc_session_open(ULONG sessionType,LPVOID sessionParams)
{	
	PSESSION_INFO_EX pSessItem;
	//PSESSION_INFO_EX tmp;
	static ULONG sess_count = 1;

	if (!by_is_inited)
		return 0;	

	if (sessionType != TRC_SESSION_LOCAL)
	{		
		return 0;
	}

	pSessItem = (PSESSION_INFO_EX)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(SESSION_INFO_EX));
	if (!pSessItem)
		return 0;

	OutputDebugString("Try to add session...\n");

	pSessItem->data.remote_id = 1;
	pSessItem->data.options = (ULONG)sessionParams;
	pSessItem->data.SessionId = sess_count++;
	pSessItem->pNext = lpFirstSessListItem;
	lpFirstSessListItem = pSessItem;
	
	OutputDebugString("session added!\n");
	return pSessItem->data.SessionId;
}


TRACERAPI VOID trc_session_close(ULONG sesId)
{
	SESSION_INFO_EX* pSessItem = lpFirstSessListItem;	
	SESSION_INFO_EX* pPred = 0;
	
	while (pSessItem)
	{
		if (pSessItem->data.SessionId == sesId)
			break;

		pPred = pSessItem;
		pSessItem = pSessItem->pNext;
	}		

	if (!pSessItem)
		return;

	if (pSessItem == lpFirstSessListItem)
		lpFirstSessListItem = pSessItem->pNext;
	else
		pSessItem->pNext = pSessItem->pNext;
	
	char sz[256];
	PBREAKPOINT_LIST pBps = pSessItem->data.pSoftBps;
	while (pBps)
	{
		wsprintf(sz, "bp set at %X\n", pBps->addr);
		OutputDebugString(sz);
		pBps = pBps->pNext;
	}

	delete_all_breakpoints(&pSessItem->data);
	dbg_detach_debugger(pSessItem->data.pDbgContext);

	delete_all_modules(&pSessItem->data);
	delete_all_threads(&pSessItem->data);
	HeapFree(hHeap, 0, pSessItem);
}


TRACERAPI VOID trc_free_list(PVOID pMem)
{
	char sz[256];
	wsprintf(sz, "Try to free mem [%X][%X]...\n", pMem, hHeap);
	OutputDebugString(sz);
	HeapFree(hHeap, 0, pMem);
	OutputDebugString("Mem freed ok!\n");
}

TRACERAPI void trc_free_mem(LPVOID mem)
{
	dbg_free_memory(mem);
}


TRACERAPI ULONG trc_get_options(ULONG SessId)
{
	SESSION_INFO* pCurSessData;

	pCurSessData = get_sess_by_id(SessId);	
	return (pCurSessData) ? pCurSessData->options : NULL;	
}


TRACERAPI ULONG trc_set_options(ULONG SessId,ULONG opt)
{
	SESSION_INFO* pCurSessData = get_sess_by_id(SessId);	
	return (pCurSessData) ? (pCurSessData->options = opt, TRUE) : FALSE;
}


TRACERAPI ULONG trc_attach_to_target(ULONG sesId,ULONG PID,ULONG attachOpts)
{
	SESSION_INFO* pCurSessData;	
	ATTACH_INFO AttachInfo;	
	CONTEXT Context;
	//PBREAKPOINT_LIST smth;
	PDBG_CONTEXT pDbgContext;
	PTHREAD_INFO_EX thrd_lst;
	PMODULE_INFO_EX mod_lst;
	EVENT_FILTER c_EventFilter;
	//ULONG bp;

	OutputDebugString("attaching to target\n");

	pCurSessData = get_sess_by_id(sesId);
	if (!pCurSessData)
		return 0;	
	
	pCurSessData->options |= attachOpts;
	pCurSessData->pDbgContext = pDbgContext = dbg_attach_debugger((PVOID)pCurSessData->remote_id, PID, (PDBGRECV)&OnEventProc, (PVOID)sesId, &AttachInfo);
	if (!pDbgContext)
		return 0;	 
		
	c_EventFilter.event_mask = -1;
	c_EventFilter.filtr_count = 0;
	OutputDebugString("debuger attached\n");
	dbg_set_filter(pCurSessData->pDbgContext, &c_EventFilter);
	dbg_set_rdtsc(pCurSessData->pDbgContext, 1);

	pCurSessData->trc_process.peb_addr = AttachInfo.peb_addr;
	pCurSessData->trc_process.PID = PID;

	thrd_lst = trc_enum_threads(sesId, PID);
	if (thrd_lst)
	{
		create_threads(pCurSessData, thrd_lst);
		dbg_free_memory(thrd_lst);		
	}

	mod_lst = dbg_enum_modules(pCurSessData->pDbgContext);
	if (mod_lst)
	{
		OutputDebugString("mod enumerated\n");
		create_modules(pCurSessData, mod_lst);
		dbg_free_memory(mod_lst);
	}

	if ((pCurSessData->options & TRC_OPT_BREAK_ON_ATTACH) && (pCurSessData->pThreads))
	{
		OutputDebugString("thread opened\n");
		dbg_suspend_thread((pCurSessData->pThreads)->data.dbg_handle);

		Context.ContextFlags = CONTEXT_FULL;
		if (dbg_get_thread_ctx((pCurSessData->pThreads)->data.dbg_handle, &Context))
		{
			OutputDebugString("setting bp\n");
			set_bp_one_shot(pCurSessData, (PVOID)Context.Eip);
			//set_bp_ex(pCurSessData, (PVOID)Context.Eip, TRC_BP_ONE_SHOT, 0);
			dbg_resume_thread((pCurSessData->pThreads)->data.dbg_handle);
			return 1;
		}
	}

	return 0;
}

TRACERAPI ULONG trc_load_target(ULONG SessId, PCHAR lpImagePath, ULONG opt)
{
	PSESSION_INFO pCurData; // eax@1
	
	PVOID remote_id; // ebx@5
	ULONG Process_id; // ebx@16
	PDBG_CONTEXT pDbgContext; // eax@17
	PTHREAD_INFO_EX thrd_lst; // eax@18
	PVOID ep_addr; // ebx@22
	unsigned long cur_tls_addr; // ebx@29
	//PMODULE_LIST main_mod; // eax@18
	//PBREAKPOINT_LIST pBp_ep; // eax@23
	ULONG tls_dir_rva; // eax@27

	signed long result; // [sp+18h] [bp-B74h]@6
	char buff[1000]; // [sp+47Ch] [bp-710h]@11
	ATTACH_INFO AttachInfo; // [sp+40h] [bp-B4Ch]@11	//	image_base
	PVOID hFile; // [sp+10h] [bp-B7Ch]@17
	EVENT_FILTER EventFilter; // [sp+864h] [bp-328h]@18	//	event_mask
//	ULONG EventFilter_filter_count; // [sp+868h] [bp-324h]@18	//	filtr_count
	PVOID image_base; // [sp+24h] [bp-B68h]@18
	DLL_LOAD_INFO dll_load; // [sp+154h] [bp-A38h]@18				//image_base
	CHAR OutputString[256]; // [sp+260h] [bp-92Ch]@18	
	PTHREAD_DATA_EX pCurThread; // [sp+14h] [bp-B78h]@19
	ULONG tls_buffer[6]; // [sp+28h] [bp-B64h]@28

	CHAR Str[256]; // [sp+378h] [bp-814h]@31

	pCurData = get_sess_by_id(SessId);
	if (!pCurData)
		return 0;

	wsprintf(OutputString, "sess opts: %X\n", pCurData->options);
	OutputDebugString(OutputString);
	OutputDebugString("loading target\n");
	pCurData->options |= opt;
	wsprintf(OutputString, "sess opts after load: %X\n", pCurData->options);
	OutputDebugString(OutputString);
	remote_id = (PVOID)pCurData->remote_id;	
	if ( remote_id == (PVOID)TRC_SESSION_INVALID )
		return 0;

	memset(&buff, 0, sizeof(buff));
	memset(&AttachInfo, 0, sizeof(AttachInfo));

	OutputDebugString("dbg_create_process\n");	
	Process_id = dbg_create_process(remote_id, (PCHAR)lpImagePath, 4);

	result = 0;

	if ( !Process_id )
		return 0;

	OutputDebugString("dbg_attach_debuger\n");
	pDbgContext = dbg_attach_debugger(remote_id, Process_id, (PDBGRECV)&OnEventProc, (PVOID)SessId, &AttachInfo);
	if ( pDbgContext )
	{
		lstrcpyn(pCurData->image_name, AttachInfo.file_name, 260);
		pCurData->trc_process.peb_addr = AttachInfo.peb_addr;
		pCurData->trc_process.PID = Process_id;

		pCurData->pDbgContext = pDbgContext;
		EventFilter.event_mask = -1;
		EventFilter.filtr_count = 0;

		image_base = AttachInfo.image_base; //	image_base
		dbg_set_filter(pCurData->pDbgContext, &EventFilter);

		lstrcpyn(dll_load.file_name, AttachInfo.file_name, 260);
		dll_load.image_base = AttachInfo.image_base;
		dll_load.image_size = AttachInfo.image_size;
		add_module(pCurData, &dll_load);

		wsprintf(OutputString, "module base: %x module size: %x fname is %s\n", AttachInfo.image_base, AttachInfo.image_size, AttachInfo.file_name);
		OutputDebugString(OutputString);		

		OutputDebugString("dbg_enum_threads\n");
		thrd_lst = dbg_enum_threads(remote_id, Process_id);
		if ( thrd_lst )
		{
			create_threads(pCurData, thrd_lst);
			pCurThread = pCurData->pThreads;
			OutputDebugString("dbg_open_thread\n");
			if ( pCurThread )
			{					
				OutputDebugString("Try to call dbg_open_file()\n");
				hFile = dbg_open_file(pDbgContext, AttachInfo.file_name, GENERIC_READ, OPEN_EXISTING);
				
				if ( hFile )
				{
					OutputDebugString("Try to call dbg_read_file()\n");
					dbg_read_file(hFile, &buff, 1000);
					if ( pCurData->options & TRC_OPT_BREAK_ON_EP )
					{
//						main_mod->ep_bp_break = 1;
						OutputDebugString("setting bp in ep\n");
						ep_addr = (PVOID)((UINT_PTR)image_base + get_ep_rva(&buff));						
						set_bp_one_shot(pCurData, ep_addr);						
					} else 
					{
						OutputDebugString("Doesn't try to set EP bp.\n");
					}
					
					//v18 = SessId;					
					if ( pCurData->options & TRC_OPT_BREAK_ON_TLS )
					{
						OutputDebugString("setting bp on Tls callbacks\n");
						tls_dir_rva = get_tls_dir(&buff, 9);						
						if ( tls_dir_rva )
						{
							OutputDebugString("Tls directory found\n");
							if ( trc_read_memory(SessId, (char*)image_base + tls_dir_rva, tls_buffer, 6*4) )
							{
								PVOID	addr;
								for ( cur_tls_addr = tls_buffer[3]; trc_read_memory(SessId, (PVOID)cur_tls_addr, &addr, 4); cur_tls_addr += 4 )
								{
									if ( !addr )
										break;
									wsprintf(Str, "setting bp on addr %#x\n", addr);
									OutputDebugString(Str);
									set_bp_one_shot(pCurData, addr);
								}
							}
						} // if ( tls_dir_rva )
					} // if ( opt & TRC_OPT_BREAK_ON_TLS )
				}
				else 
					OutputDebugString("dbg_open_file() failed!\n");
				dbg_close_file(hFile);					
				result = 1;
			} // 	if ( pCurThread )
			else 
				OutputDebugString("pCurThread == NULL!!!\n");
			dbg_resume_thread(pCurThread->data.dbg_handle);
			dbg_free_memory(thrd_lst);
		}
	}	
	return result;
}

TRACERAPI ULONG trc_set_callback(ULONG sesId,ULONG type,PVOID callback,PVOID arg)
{
	PSESSION_INFO pSessItem;

	pSessItem = get_sess_by_id(sesId);
	if (!pSessItem)
		return 0;

	OutputDebugString("callback set.\n");
	switch (type)	{
	case TRC_DBG_EVENTS_CALLBACK:
		pSessItem->DbgEvent.lpCallback = callback;
		pSessItem->DbgEvent.arg = arg;
		break;
	case TRC_PROCESS_EVENTS_CALLBACK:
		pSessItem->ProcEvent.lpCallback = callback;
		pSessItem->ProcEvent.arg = arg;
		break;
	case TRC_EXCEPTIONS_CALLBACK:
		pSessItem->ExcEvent.lpCallback = callback;
		pSessItem->ExcEvent.arg = arg;
		break;
	default: // why do not return 0?..
		return 0;
	}
	return 1;
}


PBP_LST __cdecl get_global_hws(PSESSION_INFO pSessData)
{
	PTHREAD_DATA_EX pFirst, pCur;
	PBP_LST pList;
	int thrd_count;

	pCur = pFirst = pSessData->pThreads;	
			
	thrd_count = 0;
	while (pCur)
	{
		thrd_count++;
		pCur = pCur->pNext;		
	}
	if (!thrd_count)
		return 0;
		
	pList = (PBP_LST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 4 + (thrd_count * sizeof(BP)*4));
	if (pList == NULL)
		return 0;

	pCur = pFirst;
	while (pCur) {
		for (thrd_count = 0; thrd_count < 4; thrd_count++)
		{
			if (pCur->data.breakpoint[thrd_count].bEnabled)
			{
				pList->bp[pList->count].TID = (ULONG)pCur->data.trc_thrd.TID;
				pList->bp[pList->count].type = pCur->data.breakpoint[thrd_count].type;
				pList->bp[pList->count].addr = (PVOID)pCur->data.breakpoint[thrd_count].addr;
				pList->bp[pList->count].range = pCur->data.breakpoint[thrd_count].range;
				pList->count++;
			}
		}
		pCur = pCur->pNext;
	}
	
	if (pList->count)
		return pList;
		
	HeapFree(hHeap, 0, pList);
	return 0;
}


PBP_LST __cdecl get_local_hws(PTHREAD_DATA pBps)
{
	PBP_LST pList;
	int i;

	pList = (PBP_LST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 4 + 4*sizeof(BP));
	if (pList == NULL)
		return 0;

	for(i = 0; i < 4; i++)
	{
		if (pBps->breakpoint[i].bEnabled)
		{
			pList->bp[pList->count].addr = (void*)pBps->breakpoint[i].addr;
			pList->bp[pList->count].range = pBps->breakpoint[i].range;
			pList->bp[pList->count].type = pBps->breakpoint[i].type;
			pList->bp[pList->count].TID = (ULONG)pBps->trc_thrd.TID;
			pList->count++;
		}
	}
	
	return pList;
}


TRACERAPI PSEH_LIST trc_get_seh_chain(ULONG SessId,ULONG tid)
{
	PSESSION_INFO pSessItem;
	PTHREAD_DATA thread_data;
	PVOID pCurFrame_va, pFirstFrame_va;
	ULONG frame_num;
	EXCEPTION_REGISTRATION SehRec;
	PSEH_LIST pMem;	
	int i;

	//OutputDebugString("1\n");
	pSessItem = get_sess_by_id(SessId);
	if (!pSessItem)
		return 0;
	
	//OutputDebugString("2\n");
	thread_data = get_thrd_by_tid(pSessItem, tid);
	if ((!thread_data) || (!read_memory(SessId, thread_data->trc_thrd.teb_addr, &pCurFrame_va, 4, 1)))
		return 0;
	
	//OutputDebugString("3\n");
	pFirstFrame_va = pCurFrame_va;
	for (frame_num = 0; (UINT_PTR)pCurFrame_va != -1; frame_num++)
	{
		OutputDebugString("in enum\n");
		if (!read_memory(SessId, (PVOID)pCurFrame_va, &SehRec, 8, 1))
			break;
		pCurFrame_va = SehRec.prev;
	}
	
	//OutputDebugString("4\n");
	if (frame_num == 0)
		return 0;		
	//OutputDebugString("5\n");
	pMem = (PSEH_LIST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 4+frame_num*8);
	if (pMem == NULL)
		return 0;
		
	//OutputDebugString("6\n");
	pMem->count = frame_num;
	pCurFrame_va = pFirstFrame_va;		
	
	//OutputDebugString("7\n");
	for (ULONG i = 0; i<frame_num; i++) //cycle condition not (addr != -1) but by count, calced in the past
	{
		OutputDebugString("in write\n");
		if (!read_memory(SessId, (PVOID)pCurFrame_va, &SehRec, 8, 1))
			break;
		pMem->seh[i].frame_addr = pCurFrame_va;
		pMem->seh[i].handler = SehRec.handler;
		pCurFrame_va = SehRec.prev;

		char sz[256];
		wsprintf(sz, "pMem->seh[%X].handler = %X\n", i, pMem->seh[i].handler);
		OutputDebugString(sz);
	}
	//OutputDebugString("8\n");
	return pMem;	
}

TRACERAPI bool trc_break_on_thread(ULONG SessId, ULONG tid)
{
	PSESSION_INFO pSess = get_sess_by_id(SessId);
	if (!pSess)
		return FALSE;

	PTHREAD_DATA pThrd;
	pThrd = (tid) ? get_thrd_by_tid(pSess, tid) : &pSess->pThreads->data;		
	if (!pThrd)
		return FALSE;

	dbg_suspend_thread(pThrd->dbg_handle);

	CONTEXT con;
	con.ContextFlags = CONTEXT_FULL;
	dbg_get_thread_ctx(pThrd->dbg_handle, &con);
	UINT_PTR addr = con.Eip;
	if (!set_bp_ex(pSess, (PVOID)addr, TRC_BP_THREAD, tid, TRC_EVENT_BREAKPOINT))
		return FALSE;
	dbg_resume_thread(pThrd->dbg_handle);

	return TRUE;
}


//#define IMAGE_EP(x) (PVOID)(((PIMAGE_NT_HEADERS)((char*)(x) + ((PIMAGE_DOS_HEADER)(x))->e_lfanew))->OptionalHeader.AddressOfEntryPoint +(char*)(x))