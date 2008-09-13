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


/**/
void __cdecl create_threads(SESSION_INFO* pSessData, PTHREAD_INFO_EX pThreadInfoEx)
{		
	THREAD_START thrd_start;
	ULONG i;
	char sz[256];

	OutputDebugString("==create_threads==\n");
	for (i = 0; i < pThreadInfoEx->thrd_count; i++)
	{		
		
		wsprintf(sz, " add thread tid:%X, teb:%X", pThreadInfoEx->threads[i].thread_id, pThreadInfoEx->threads[i].teb_addr);
		OutputDebugString(sz);

		thrd_start.teb_addr = pThreadInfoEx->threads[i].teb_addr;
		thrd_start.thread_id = pThreadInfoEx->threads[i].thread_id;
		add_thread(pSessData, &thrd_start);		
	}
}
/**/

//sub_10001AE0 - add thread?

/**/
PTHREAD_DATA_EX __cdecl add_thread(PSESSION_INFO pSessData, PTHREAD_START pSmth)
{
	PTHREAD_DATA_EX pNew;
	PTHREAD_DATA_EX res = (PTHREAD_DATA_EX)dbg_open_thread(pSessData->pDbgContext, (ULONG)pSmth->thread_id);

	if (!res)
		return res;

	pNew = (PTHREAD_DATA_EX)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(THREAD_DATA_EX));
	pNew->data.trc_thrd.TID = pSmth->thread_id;
	pNew->data.trc_thrd.teb_addr = pSmth->teb_addr;
	pNew->data.dbg_handle = res;
	//pNew->data.EnableDrAddr = -1;

	pNew->pNext = pSessData->pThreads;
	pSessData->pThreads = pNew;
	return pNew;
}

/**/


//OK
//10001BA0 - get_thrd_list
PTHREAD_DATA __cdecl get_thrd_by_tid(PSESSION_INFO pSessDataStruct, ULONG tid)
{
	PTHREAD_DATA_EX pFirst, pCur;
	char OutputStr[256];
	wsprintf(OutputStr, "ThreadFromId(%x,%x)\n", pSessDataStruct, tid);
	OutputDebugString(OutputStr);

	pCur = pSessDataStruct->pThreads;
	while (pCur)
	{
		wsprintf(OutputStr, "%x - %x\n", pCur->data.trc_thrd.TID, tid);
		//OutputDebugString(OutputStr);			
		if (pCur->data.trc_thrd.TID == tid)
		{
			OutputDebugString("  Thread Found\n");
			return &pCur->data;
		}
		pCur = pCur->pNext;
	};

	OutputDebugString("  Thread Not Found\n");
	return NULL;
}


//----- (10001A70) --------------------------------------------------------
ULONG __cdecl delete_thread(PSESSION_INFO pSessData, PTHREAD_EXIT thrd_exit)
{
	PTHREAD_DATA_EX pCurThread; // esi@1	
	PTHREAD_DATA_EX *pPrev; // ecx@5

	pCurThread = pSessData->pThreads;
	pPrev = &pSessData->pThreads;
	while (pCurThread)
	{		
		if ( pCurThread->data.trc_thrd.TID == thrd_exit->thread_id)
			break;
		pCurThread = pCurThread->pNext;
		pPrev = &pCurThread->pNext;
	}
	if (!pCurThread)
		return 0;

	*pPrev = pCurThread->pNext;
	dbg_close_thread(pCurThread->data.dbg_handle);
	HeapFree(hHeap, 0, pCurThread);	
	return 0;
}

void delete_all_threads(PSESSION_INFO pSess)
{
	PTHREAD_DATA_EX pOld, pCurThrd = pSess->pThreads;
	
	while (pCurThrd)
	{
		pOld = pCurThrd;
		pCurThrd = pOld->pNext;
		dbg_close_thread(pOld->data.dbg_handle);
		HeapFree(hHeap, 0, pOld);	
	}
	pSess->pThreads = NULL;
}


//------------------------------------------------------------------------
TRACERAPI PTHREAD_INFO_EX trc_enum_threads(ULONG sesId, ULONG PID)
{
	PSESSION_INFO CurrSessItem = get_sess_by_id(sesId);	
	if (!CurrSessItem)
		return 0;

	if (CurrSessItem->remote_id != -1)
		return dbg_enum_threads((PVOID)CurrSessItem->remote_id, PID);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//Funcs of copying contexts to increase speed of setting/getting contexts
//

int Con2TrcFrame(PTRC_REG_FRAME frm, PCONTEXT ctx)
{
	frm->Dr0 = ctx->Dr0;
	frm->Dr1 = ctx->Dr1;
	frm->Dr2 = ctx->Dr2;
	frm->Dr3 = ctx->Dr3;
	frm->Dr6 = ctx->Dr6;
	frm->Dr7 = ctx->Dr7;


	frm->SegGs = ctx->SegGs;
	frm->SegEs = ctx->SegEs;
	frm->SegDs = ctx->SegDs;

	frm->Edx = ctx->Edx;
	frm->Ecx = ctx->Ecx;
	frm->Eax = ctx->Eax;
	frm->SegFs = ctx->SegFs;

	frm->Edi = ctx->Edi;
	frm->Esi = ctx->Esi;
	frm->Ebx = ctx->Ebx;
	frm->Ebp = ctx->Ebp;
	frm->Esp = ctx->Esp;


	frm->Eip = ctx->Eip;
	frm->SegCs = ctx->SegCs;
	frm->EFlags = ctx->EFlags;

	return 1;
}

int TrcFrame2Con(PCONTEXT ctx, PTRC_REG_FRAME frm)
{
	ctx->Dr0 = frm->Dr0;
	ctx->Dr1 = frm->Dr1;
	ctx->Dr2 = frm->Dr2;
	ctx->Dr3 = frm->Dr3;
	ctx->Dr6 = frm->Dr6;
	ctx->Dr7 = frm->Dr7;

	ctx->SegGs = frm->SegGs;
	ctx->SegEs = frm->SegEs;
	ctx->SegDs = frm->SegDs;

	ctx->Edx = frm->Edx;
	ctx->Ecx = frm->Ecx;
	ctx->Eax = frm->Eax;
	ctx->SegFs = frm->SegFs;

	ctx->Edi = frm->Edi;
	ctx->Esi = frm->Esi;
	ctx->Ebx = frm->Ebx;
	ctx->Ebp = frm->Ebp;
	ctx->Esp = frm->Esp;


	ctx->Eip = frm->Eip;
	ctx->SegCs = frm->SegCs;
	ctx->EFlags = frm->EFlags;
	return 1;
}

//------------------------------------------------------------------------
TRACERAPI ULONG trc_set_thread_ctx(ULONG sesId,ULONG TID,PCONTEXT ctx)
{
	PSESSION_INFO CurrSessItem;
	PTHREAD_DATA pThreadPrivData;
	//int result = 0;

	CurrSessItem = get_sess_by_id(sesId);
	if (!CurrSessItem)
		return 0;

	pThreadPrivData = get_thrd_by_tid(CurrSessItem, TID);
	if (pThreadPrivData)
	{
		//if (pThreadPrivData->pFastContext)
		return Con2TrcFrame(pThreadPrivData->pFastContext, ctx);
		//else
			//return dbg_set_thread_ctx(pThreadPrivData->dbg_handle, ctx);
	}
	return 0;
}
//------------------------------------------------------------------------
TRACERAPI ULONG trc_get_thread_ctx(ULONG sesId,ULONG TID,PCONTEXT ctx)
{
	PSESSION_INFO CurrSessItem;
	int result = 0;

	CurrSessItem = get_sess_by_id(sesId);

	if (!CurrSessItem)
		return 0;

	PTHREAD_DATA pThreadPrivData = get_thrd_by_tid(CurrSessItem, TID);
	if (pThreadPrivData)
	{
		if (pThreadPrivData->pFastContext)
			return TrcFrame2Con(ctx, pThreadPrivData->pFastContext);
		else
			return dbg_get_thread_ctx(pThreadPrivData->dbg_handle, ctx);
	}
	return 0;
}


TRACERAPI ULONG trc_suspend_thread(ULONG sesId, ULONG TID)
{
	PSESSION_INFO pSess = get_sess_by_id(sesId);
	if (!pSess)
		return 0;

	PTHREAD_DATA CurThrd = get_thrd_by_tid(pSess, TID);
	if (!CurThrd)
		return 0;
	
	dbg_suspend_thread(CurThrd->dbg_handle);
	return 1;
}

TRACERAPI ULONG trc_resume_thread(ULONG sesId, ULONG TID)
{
	PSESSION_INFO pSess = get_sess_by_id(sesId);
	if (!pSess)
		return 0;

	PTHREAD_DATA CurThrd = get_thrd_by_tid(pSess, TID);
	if (!CurThrd)
		return 0;

	dbg_resume_thread(CurThrd->dbg_handle);
	return 1;
}


TRACERAPI ULONG trc_terminate_process(ULONG sesId, ULONG PID)
{
	PSESSION_INFO pSess = get_sess_by_id(sesId);
	if (!pSess)
		return 0;

	return dbg_terminate_process((PVOID)pSess->remote_id, PID);
};

PEXCEP_FRAME create_except_frame(PSESSION_INFO pSess, PTHREAD_DATA pThread)
{
	PEXCEP_FRAME pFrame;
	char sz[256];
	pFrame = ALLOC(EXCEP_FRAME);

	pFrame->bDebugTraced = pThread->bDbgTraced;	
	pFrame->TraceMode = pThread->TraceMode;
	wsprintf(sz, "Frame: bDbg=%X, mode=%X\n", pFrame->bDebugTraced, pFrame->TraceMode);
	//OutputDebugString(sz);

	pFrame->pSehs = trc_get_seh_chain(pSess->SessionId, pThread->trc_thrd.TID);
	if (!pFrame->pSehs)
	{
		pFrame->pSehs = ALLOC(SEH_LIST);		
	}

	
	wsprintf(sz, "trc_get_seh_chain returned %X\n", pFrame->pSehs);
//	OutputDebugString(sz);

	pFrame->pPrev = pThread->pExcFrames;
	pThread->pExcFrames = pFrame;	
	return pFrame;
}