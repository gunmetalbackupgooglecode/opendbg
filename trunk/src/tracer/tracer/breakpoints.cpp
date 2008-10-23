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


void check_bp_exists(PSESSION_INFO pSess, PBREAKPOINT_LIST pBp, PBREAKPOINT_LIST* pPointer);
void delete_tid(PBREAKPOINT_LIST pBp, ULONG TID);

bool set_thrd_hard_bp(PSESSION_INFO pSess, PTHREAD_DATA pThrd, PVOID addr, ULONG type, ULONG range);
ULONG set_hard_bp(ULONG SessId, ULONG tid, int bp_type, ULONG range, PVOID addr);

bool delete_hard_bp(PSESSION_INFO pSess, ULONG tid, PVOID addr, ULONG type, ULONG range);
bool delete_hw(PTHREAD_DATA pThrd, ULONG num);

void add_bp_tid(PTID_EX* pBp, ULONG TID)
{
	PTID_EX pCur = *pBp;
	while (pCur)
	{
		if (pCur->tid == TID)
			return;
		pCur = pCur->pNext;
	}

	pCur = (PTID_EX)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(TID_EX));
	pCur->tid = TID;

	pCur->pNext = *pBp;
	*pBp = pCur;
}

bool search_tid(PTID_EX pBp, ULONG TID )
{
	PTID_EX pCur = pBp;
	while (pCur)
	{
		if (pCur->tid == TID)
			return TRUE;
		pCur = pCur->pNext;
	}
	return FALSE;
}

void delete_tid(PTID_EX* pBp, ULONG TID)
{
	PTID_EX* pPred = pBp;
	PTID_EX pCur = *pBp;
	char sz[256];

	wsprintf(sz, "delete_tid(%X, %X)\n", pBp, TID);
	OutputDebugString(sz);

	while (pCur)
	{
		wsprintf(sz, " --pCur = %X, pNext = %X, tid = %X\n", pCur, pCur->pNext, pCur->tid);
		OutputDebugString(sz);
		if (pCur->tid == TID)
		{
			*pPred = pCur->pNext;
			HeapFree(hHeap, 0, pCur);
			return;
		}	
		pPred = &pCur->pNext;
		pCur = pCur->pNext;
	}
}

PBREAKPOINT_LIST create_bp(PSESSION_INFO pSess, PVOID addr)
{
	static UCHAR l_cc = 0xCC;
	PBREAKPOINT_LIST pCur = pSess->pSoftBps;
	char sz[256];
	wsprintf(sz, "create_bp(%X, %X)\n", pSess, addr);
	OutputDebugString(sz);

	while (pCur)
	{
		if (pCur->addr == addr)
		{
			OutputDebugString("bp already exists\n");
			return pCur;
		}
		pCur = pCur->pNext;
	}

	//OutputDebugString("1\n");
	pCur = (PBREAKPOINT_LIST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(BREAKPOINT_LIST));
	pCur->addr = addr;
	pCur->pNext = pSess->pSoftBps;
	pSess->pSoftBps = pCur;

	wsprintf(sz, "bp created: pCur = %X, pNext = %X, addr = %X\n", pCur, pCur->pNext, pCur->addr);
	OutputDebugString(sz);
	

	//OutputDebugString("Try to set bp...");
	// FIXME: OLOLO! sucky condition, sucky code?
	/*
	if ((pSess->options | TRC_OPT_HIDE_BREAKPOINTS) && 0)
	{		
		pCur->pHookedMem = VirtualAlloc(0, 0x1000, 0x3000, PAGE_EXECUTE_READWRITE);

		if (pCur->pHookedMem)
		{
			if (dbg_read_memory(pSess->pDbgContext, addr, &pCur->SavedByte, 1, 1))
			{
				if (dbg_read_memory(pSess->pDbgContext, (PVOID)((UINT_PTR)addr & 0xFFFFF000), pCur->pHookedMem, 0x1000, 1))
				{
					((BYTE*)pCur->pHookedMem)[(UINT_PTR)addr & 0xFFF] = l_cc; // set int3 here
					if (dbg_hook_page(pSess->pDbgContext, (PVOID)((UINT_PTR)addr & 0xFFFFF000), pCur->pHookedMem))
					{
						OutputDebugString("hidden bp set\n");
						return pCur;
					}
				}  else OutputDebugString("dbg_write_memory returned 0\n");
			} else OutputDebugString("dbg_write_memory returned 0\n");
			
			VirtualFree(pCur->pHookedMem, 0x1000, MEM_RELEASE);
		}

	}
	else
	*/
	{// if (options | TRC_OPT_HIDE_BREAKPOINTS)
		//not hidden
		//OutputDebugString("3\n");
		//wsprintf(sz, "dbg_read_memory(%X, %X, %X, %X, %X)", pSess->pDbgContext, addr, &pCur->SavedByte, 1, 1);
		//OutputDebugString(sz);
		if (dbg_read_memory(pSess->pDbgContext, addr, &pCur->SavedByte, 1, 1))
		{
			//OutputDebugString("4\n");
			if (dbg_write_memory(pSess->pDbgContext, addr, &l_cc, 1, 1))
			{
				//OutputDebugString("5\n");
				OutputDebugString("bp set\n");
				return pCur;
			} else OutputDebugString("dbg_write_memory returned 0\n");
		} else OutputDebugString("dbg_read_memory returned 0\n");
	}

	OutputDebugString("Breakpoint is not set!\n");
	//pSess->pSoftBps = pCur->pNext;
	HeapFree(hHeap, 0, pCur);
	return NULL;
}


bool set_bp_user(PSESSION_INFO pSess, PVOID addr)
{
	return set_bp_ex(pSess, addr, TRC_BP_USER, 0, TRC_EVENT_BREAKPOINT);	
}

bool set_bp_one_shot(PSESSION_INFO pSess, PVOID addr)
{
	return set_bp_ex(pSess, addr, TRC_BP_ONE_SHOT, 0, TRC_EVENT_BREAKPOINT);	
}



bool set_bp_tid(PSESSION_INFO pSess, PVOID addr, ULONG TID)
{
	return set_bp_ex(pSess, addr, TRC_BP_THREAD, TID, TRC_EVENT_BREAKPOINT);
}


void check_bp_exists(PSESSION_INFO pSess, PBREAKPOINT_LIST pBp, PBREAKPOINT_LIST* pPointer)
{
	char sz[256];
	if (!pBp->Kind)
	{
		wsprintf(sz, "check_bp_exists(%X, %X, %X), addr = %X, next = %X", pSess, pBp, pPointer, pBp->addr, pBp->pNext);
		OutputDebugString(sz);
		dbg_write_memory(pSess->pDbgContext, pBp->addr, &pBp->SavedByte, sizeof(BYTE), 1);
		*pPointer = pBp->pNext;
		HeapFree(hHeap, 0, pBp);
	}
}


void delete_bp_user(PSESSION_INFO pSess, PVOID addr)
{
	delete_bp(pSess, addr, 0, TRC_BP_USER);
}



void delete_bp_one_shot(PSESSION_INFO pSess, PVOID addr)
{
	delete_bp(pSess, addr, 0, TRC_BP_ONE_SHOT);
}


void delete_all_breakpoints(PSESSION_INFO pSess)
{
	PBREAKPOINT_LIST pOld, pCur = pSess->pSoftBps;
	char sz[256];
	
	while (pCur)
	{
		wsprintf(sz, "delete bp at %X\n", pCur->addr);
		OutputDebugString(sz);
		pOld = pCur;
		dbg_write_memory(pSess->pDbgContext, (PVOID)pOld->addr, &pOld->SavedByte, sizeof(BYTE), 1);
		pCur = pOld->pNext;
		HeapFree(hHeap, 0, pOld);		
	}
}

//~
TRACERAPI ULONG trc_set_bp(ULONG sesId,ULONG bp_type,ULONG TID,PVOID addr,ULONG range)
{
	PSESSION_INFO pSess = get_sess_by_id(sesId);
	if (!pSess)
		return 0;

	char str[256];
	wsprintf(str, "trc_set_bp(%x,%x,%x,%x,%x)\n", sesId, bp_type, TID, addr, range);
	OutputDebugString(str);

	switch (bp_type) {
	case TRC_BP_SOFTWARE:		
		return set_bp_user(pSess, addr);
	case TRC_HWBP_READWRITE:
	case TRC_HWBP_WRITE:
	case TRC_HWBP_EXECUTE:
		return set_hard_bp(sesId, TID, bp_type, range, addr);
	case TRC_BP_ONE_SHOT:
		return set_bp_one_shot(pSess, addr);
	default:
		return 0;
	}
}

void restore_bp(PSESSION_INFO pSessData, PTHREAD_DATA pThrd, PVOID addr)
{
	UCHAR byCC = 0xCC;

	if ( (pSessData->CC_need_addr) && (pSessData->CC_need_addr != addr))	// step after int3 break to rewrite CC
	{
		dbg_write_memory(pSessData->pDbgContext, (PVOID)pSessData->CC_need_addr, &byCC, 1, 1);
		pSessData->CC_need_addr = 0;	
	}

	if ( (pThrd->EnableDrAddr) && (pThrd->EnableDrAddr != addr))
	{
		trc_enable_bp(pSessData->SessionId, TRC_HWBP_EXECUTE, pThrd->EnableDrAddr, pThrd->trc_thrd.TID, TRUE);
		pThrd->EnableDrAddr = 0;
	}
}

TRACERAPI ULONG trc_delete_bp(ULONG sesId,ULONG TID,PVOID addr, ULONG bp_type_mask)
{	
	PSESSION_INFO pSess = get_sess_by_id(sesId);
	if (!pSess)
		return 0;

	bp_type_mask = (bp_type_mask) ? (bp_type_mask) :
		(TRC_BP_SOFTWARE | TRC_BP_ONE_SHOT | TRC_HWBP_READWRITE | TRC_HWBP_WRITE | TRC_HWBP_EXECUTE | TRC_HWBP_IO);

	if (bp_type_mask & TRC_BP_ONE_SHOT)
		delete_bp_one_shot(pSess, addr);

	if (bp_type_mask & TRC_BP_SOFTWARE)
		delete_bp_user(pSess, addr);

	delete_hard_bp(pSess, TID, addr, bp_type_mask, 1);
	delete_hard_bp(pSess, TID, addr, bp_type_mask, 2);
	delete_hard_bp(pSess, TID, addr, bp_type_mask, 4);
	delete_hard_bp(pSess, TID, addr, bp_type_mask, 8);
	return 0;
}




TRACERAPI PBP_LST trc_get_bp_list(ULONG sesId,ULONG TID)
{
	PBP_LST pSoft = get_soft_bp_lst(sesId);
	PBP_LST pHard = get_hard_bp_lst(sesId, TID);
	PBP_LST pBpList;
	ULONG size = 0, i, j;	

	char sz[256];
	wsprintf(sz, "got bps: %X, %X\n", pSoft, pHard);

	OutputDebugString(sz);
	if (pSoft)
		size = pSoft->count * 16;

	if (pHard)
		size += pHard->count * 16;

	if (!size)
		return 0;

	size += 4;

	pBpList = (PBP_LST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, size);
	if (pBpList == NULL)
		return 0;

	i = 0;
	if (pSoft)
	{
		for (; i < pSoft->count; i++)
		{	
			pBpList->bp[i].TID = pSoft->bp[i].TID;
			pBpList->bp[i].type = pSoft->bp[i].type;
			pBpList->bp[i].addr = pSoft->bp[i].addr;
			pBpList->bp[i].range = pSoft->bp[i].range;
		}
		pBpList->count = i;		
		HeapFree(hHeap, 0, pSoft);
	}

	if (pHard == NULL)
		return pBpList;

	for (j = 0; j < pHard->count; j++, i++)
	{

		pBpList->bp[i].TID = pHard->bp[j].TID;
		pBpList->bp[i].type = pHard->bp[j].type;
		pBpList->bp[i].addr = pHard->bp[j].addr;
		pBpList->bp[i].range = pHard->bp[j].range;
	}
	pBpList->count = i;

	HeapFree(hHeap, 0, pHard);
	return pBpList;
}


PBP_LST get_soft_bp_lst(ULONG SessId)
{
	PSESSION_INFO pSessItem;
	PBREAKPOINT_LIST pBpList;
	PBP_LST pList;
	ULONG bp_count, i;

	pSessItem = get_sess_by_id(SessId);
	if (!pSessItem)
		return 0;

	//enums bp count
	pBpList = pSessItem->pSoftBps;	
	for (bp_count = 0; pBpList; pBpList = pBpList->pNext)
	{
		//if (pBpList->bEngineBp) // enabled?
		bp_count++;		
	}
	if (bp_count == 0)	//no bps
		return 0;	

	//create list
	pList = (PBP_LST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, bp_count*16 + 4);
	if (pList == NULL)
		return NULL;

	pList->count = 0;
	pBpList = pSessItem->pSoftBps;

	for (i = 0; pBpList; pBpList = pBpList->pNext)
	{
		if (pBpList->Kind & (TRC_BP_USER | TRC_BP_ONE_SHOT))
		{
			pList->bp[i].addr = pBpList->addr;
			pList->bp[i].type = pBpList->Kind;
			i++;
			pList->count++;
		}		
	}	

	return pList;
}


PBP_LST get_hard_bp_lst(ULONG SessId, ULONG tid)
{
	PSESSION_INFO pSessItem;
	PTHREAD_DATA res;

	pSessItem = get_sess_by_id(SessId);
	if (!pSessItem)
		return 0;

	if (!tid)
		return get_global_hws(pSessItem);

	res = get_thrd_by_tid(pSessItem, tid);
	if (res == NULL) // no thread with such tid
		return 0;

	return get_local_hws(res);	
}


//----- (10001970) --------------------------------------------------------
PBREAKPOINT_LIST find_soft_bp_by_addr(PBREAKPOINT_LIST pBps, PVOID addr)
{
	PBREAKPOINT_LIST pCurBp;	
	CHAR OutputString[256];

	pCurBp = pBps;	
	wsprintf(OutputString, "FindBP(%X,%X);\n", pBps, addr);
	OutputDebugString(OutputString);
	while (pCurBp)
	{
		wsprintf(OutputString, "  --pCur = %X, pCur->pNext = %X, addr = %X\n", pCurBp, pCurBp->pNext, pCurBp->addr);
		//OutputDebugString(OutputString);
		if (pCurBp->addr == addr)
		{
			OutputDebugString("  Found\n");
			return pCurBp;
		}	
		pCurBp = pCurBp->pNext;
	}

	OutputDebugString("  Not found\n");
	return 0;
}





//////////////////////////////////////////////////////////////////////////
//Enable/disable bp functions
//

void add_disable_bp(PSESSION_INFO pSess, ULONG bp_type, PVOID addr, ULONG TID, ULONG range)
{
	PBP_EX pBp = ALLOC(BP_EX);
	pBp->pNext = pSess->pDisabledBps;
	pSess->pDisabledBps = pBp;

	pBp->data.addr = addr;
	pBp->data.range = range;
	pBp->data.TID = TID;
	pBp->data.type = bp_type;	
}

BOOL set_disabled_bp(PSESSION_INFO pSess, ULONG bp_type, PVOID addr, ULONG TID)
{
	BOOL res = FALSE;
	PBP_EX pCurBp = pSess->pDisabledBps;
	PBP_EX* pPrev = &pSess->pDisabledBps;
	char sz[256];

	wsprintf(sz, "set_disabled_bp(%X, %X, %X, %X)", pSess, bp_type, addr, TID);
	OutputDebugString(sz);

	while (pCurBp)
	{
		wsprintf(sz, "at %X - %X, addr=%X, type=%X\n", pCurBp);
		if ((pCurBp->data.type & bp_type) &&
			(pCurBp->data.addr == addr) &&
			((!TID) || (pCurBp->data.TID == TID)))
		{
			trc_set_bp(pSess->SessionId, pCurBp->data.type, pCurBp->data.TID, pCurBp->data.addr, pCurBp->data.range);
			*pPrev = pCurBp->pNext;
			FREE(pCurBp);
			res = TRUE;
			pCurBp = *pPrev;
			continue;
		}
		pPrev = &pCurBp->pNext;
		pCurBp = pCurBp->pNext;
	}
	return res;
}

void disable_thread_hw(PSESSION_INFO pSess, PTHREAD_DATA pThrd, ULONG bp_type, PVOID addr, ULONG /*TID*/)
{
	int i;
	for (i=0; i<4; i++)
	{
		if ((pThrd->breakpoint[i].bEnabled) &&
			(pThrd->breakpoint[i].addr == addr) &&
			(pThrd->breakpoint[i].type & bp_type))
		{
			OutputDebugString("Found hard bp to disable.\n");
			add_disable_bp(pSess, pThrd->breakpoint[i].type, addr, pThrd->trc_thrd.TID, pThrd->breakpoint[i].range);
			trc_delete_bp(pSess->SessionId, pThrd->trc_thrd.TID, addr, pThrd->breakpoint[i].type);										
		}
	}
}

TRACERAPI void trc_enable_bp(ULONG SessId, ULONG bp_type, PVOID addr, ULONG TID, BOOL bEnable)
{
	PSESSION_INFO pSess = get_sess_by_id(SessId);
	if (!pSess)
		return;

	char sz[256];
	wsprintf(sz, "trc_enable_bp(%X, %X, %X, %X, %X)\n", SessId, bp_type, addr, TID, bEnable);
	OutputDebugString(sz);

	if (bEnable)
	{
		set_disabled_bp(pSess, bp_type, addr, TID);
	}
	else
	{ //Disable
		if (bp_type & TRC_BP_SOFTWARE)
		{
			if (find_soft_bp_by_addr(pSess->pSoftBps, addr))
			{
				OutputDebugString("Found soft bp to disable.\n");
				add_disable_bp(pSess, TRC_BP_SOFTWARE, addr, 0, 0);				
				trc_delete_bp(SessId, 0, addr, TRC_BP_SOFTWARE);				
			}
		}
		
		
		if (TID == 0)
		{ // all threads				
			PTHREAD_DATA_EX pThrd;			
			pThrd = pSess->pThreads;
			while (pThrd)
			{
				disable_thread_hw(pSess, &pThrd->data, bp_type, addr, TID);
				pThrd = pThrd->pNext;
			}
		}
		else // only one thread
		{
			PTHREAD_DATA pThrd;
			pThrd = get_thrd_by_tid(pSess, TID);
			if (pThrd)
			{
				disable_thread_hw(pSess, pThrd, bp_type, addr, TID);
			}
		}		
	}	
}

TRACERAPI PBP_LST trc_get_disabled_bp_list(ULONG SessId, ULONG TID)
{
	PBP_EX pCurBp;
	int count = 0;
	PSESSION_INFO pSess = get_sess_by_id(SessId);
	if (!pSess)
		return 0;

		
	//get count
	pCurBp = pSess->pDisabledBps;
	while (pCurBp)
	{
		if ((pCurBp->data.TID == TID) || (!TID))
			count++;
		pCurBp = pCurBp->pNext;
	}

	if (!count)
		return 0;
	PBP_LST pList = (PBP_LST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, 4 + count*sizeof(BP));
	pList->count = count;

	pCurBp = pSess->pDisabledBps;
	while (pCurBp)
	{
		if ((pCurBp->data.TID == TID) || (!TID))
		{			
			memcpy(&pList->bp[--count], &pCurBp->data, sizeof(BP));			
		}
		pCurBp = pCurBp->pNext;
	}
	return pList;
}


//////////////////////////////////////////////////////////////////////////
//base bp functions


bool set_bp_ex(PSESSION_INFO pSess, PVOID addr, ULONG kind, ULONG tid, ULONG event_code)
{
	PBREAKPOINT_LIST pBp = find_soft_bp_by_addr(pSess->pSoftBps, addr);
	if (!pBp)
		pBp = create_bp(pSess, addr);

	if (!pBp)
		return FALSE;

	pBp->event_code = event_code;
	pBp->Kind |= kind;
	switch (kind) {
	case TRC_BP_THREAD:
		add_bp_tid(&pBp->pTidList, tid);
		break;
	case TRC_BP_TIL_RET:
		add_bp_tid(&pBp->pTilRetTidList, tid);
		break;
	case TRC_BP_SHADOW:
		add_bp_tid(&pBp->pShadowTidList, tid);
		break;
	}
	return TRUE;
}

ULONG process_bp_break(PSESSION_INFO pSess, PVOID addr, ULONG tid, PULONG lpEvCode)
{
	OutputDebugString("process_bp_break\n");
	PBREAKPOINT_LIST pBp = find_soft_bp_by_addr(pSess->pSoftBps, addr);
	if (!pBp)
		return 0;

	*lpEvCode = pBp->event_code;
	ULONG res;
	ULONG Kind = pBp->Kind;

	char sz[256];
	wsprintf(sz, "  Break Kind = %X\n", Kind);
	OutputDebugString(sz);

	//////////////////////////////////////////////////////////////////////////
	//user break || one-shot
	res = (Kind & TRC_BP_USER) || (Kind & TRC_BP_ONE_SHOT);	
	delete_bp(pSess, addr, tid, TRC_BP_ONE_SHOT);
	//trace bp - on thread
	if (Kind & TRC_BP_THREAD)
	{
		bool result = search_tid(pBp->pTidList, tid);
		delete_bp(pSess, addr, tid, TRC_BP_THREAD);
		if (result)
		{
			OutputDebugString("  TRC_BP_USER\n");		
			return TRC_BP_USER;
		}
	}

	OutputDebugString("ret try");
	//run till retn
	if ((Kind & TRC_BP_TIL_RET) && (search_tid(pBp->pTilRetTidList, tid)))
	{
		delete_bp(pSess, addr, tid, TRC_BP_TIL_RET);
		OutputDebugString("  TRC_BP_TIL_RET\n");
		return TRC_BP_TIL_RET;
	}

	OutputDebugString("Shadow try");
	//shadow
	if ((Kind & TRC_BP_SHADOW) && (search_tid(pBp->pShadowTidList, tid)))
	{
		delete_bp(pSess, addr, tid, TRC_BP_SHADOW);
		OutputDebugString("  TRC_BP_SHADOW\n");
		return TRC_BP_SHADOW;
	}
	
	return 0;
}

void delete_bp(PSESSION_INFO pSess, PVOID addr, ULONG tid, ULONG type_mask)
{
	PBREAKPOINT_LIST pBp = find_soft_bp_by_addr(pSess->pSoftBps, addr);
	if (!pBp)
		return;	

	pBp->Kind &= ~(type_mask & (TRC_BP_USER | TRC_BP_ONE_SHOT));

	if (type_mask & TRC_BP_THREAD)
	{
		delete_tid(&pBp->pTidList, tid);
		if (!pBp->pTidList)
			pBp->Kind &= ~TRC_BP_THREAD;
	}

	if (type_mask & TRC_BP_TIL_RET)
	{
		delete_tid(&pBp->pTilRetTidList, tid);
		if (!pBp->pTilRetTidList)
			pBp->Kind &= ~TRC_BP_TIL_RET;
	}

	if (type_mask & TRC_BP_SHADOW)
	{
		delete_tid(&pBp->pShadowTidList, tid);
		if (!pBp->pShadowTidList)
			pBp->Kind &= ~TRC_BP_SHADOW;
	}

	PBREAKPOINT_LIST* pBpLst = &pSess->pSoftBps;
	while (*pBpLst)
	{
		if (*pBpLst == pBp)
			break;
		pBpLst = &(*pBpLst)->pNext;
	}
	if (*pBpLst)
		check_bp_exists(pSess, pBp, pBpLst);
}




//////////////////////////////////////////////////////////////////////////
//Hardware bps
//

ULONG set_hard_bp(ULONG SessId, ULONG tid, int bp_type, ULONG range, PVOID addr)
{
	PSESSION_INFO pSessItem;	
	PTHREAD_DATA_EX pThrd;
	OutputDebugString("set_hard_bp\n");
	
	pSessItem = get_sess_by_id(SessId);
	if (!pSessItem)
		return 0;
	if ((range != 1) && (range != 2) && (range != 4))
		range = 1;
	if (bp_type == TRC_HWBP_EXECUTE)
		range = 1;

	OutputDebugString("  set_hard_bp(2)\n");
	if (tid)
	{
		PTHREAD_DATA pThrd = get_thrd_by_tid(pSessItem, tid);
		if (!pThrd)
			return 0;
		return set_thrd_hard_bp(pSessItem, pThrd, addr, bp_type, range);
	}

	//set hws on all threads
	int res = 0;		
	pThrd = pSessItem->pThreads;
	while (pThrd)
	{
		if (set_thrd_hard_bp(pSessItem, &pThrd->data, addr, bp_type, range))
			res++;
		pThrd = pThrd->pNext;
	}
	return res; // zero if no threads
}

int add_hard_bp(PTHREAD_DATA pThrd, PVOID addr, ULONG type, ULONG range)
{
	int i;
	for (i=0; i<4; i++)
	{
		if ((pThrd->breakpoint[i].bEnabled)
			&& (pThrd->breakpoint[i].addr == addr)
			&& (pThrd->breakpoint[i].range == range)
			&& (pThrd->breakpoint[i].type == type))
			return i;
	}

	for (i=0; ;i++)
	{
		if (!pThrd->breakpoint[i].bEnabled)
			break;
		if (i >= 3)
			return -1;
	}

	pThrd->breakpoint[i].bEnabled = 1;
	pThrd->breakpoint[i].addr = addr;
	pThrd->breakpoint[i].range = range;
	pThrd->breakpoint[i].type = type;
	return i;
}

void set_dr7(PULONG pDr7, ULONG num, ULONG type, ULONG range)
{
	ULONG val;
	ULONG dr7 = *pDr7;

	char sz[256];
	wsprintf(sz, "Dr7 before = %X\n", dr7);
	OutputDebugString(sz);
	//zero it
	dr7 &= ~((15 << (16 + num*4)) | (1 << (num*2)));

	wsprintf(sz, "Dr7 1 = %X\n", dr7);
	OutputDebugString(sz);
	//type
	switch (type) {
	case TRC_HWBP_READWRITE: val = 3; break;
	case TRC_HWBP_WRITE: val = 1; break;
	case TRC_HWBP_EXECUTE: val = 0; break;
	case TRC_HWBP_IO: val = 2; break;
	}
	dr7 |= val << (16 + 4*num);

	wsprintf(sz, "Dr7 2 = %X\n", dr7);
	OutputDebugString(sz);
	//range
	switch (range)
	{
	case 1:	val = 0; break;
	case 2:	val = 1; break;
	case 4:	val = 3; break;
	case 8:	val = 2; break;	
	}

	wsprintf(sz, "Dr7 3 = %X\n", dr7);
	OutputDebugString(sz);
	dr7 |= val << (18 + 4*num);
	dr7 |= 0x500;
	dr7 |= 1 << (2*num);

	wsprintf(sz, "Dr7 after = %X\n", dr7);
	OutputDebugString(sz);
	*pDr7 = dr7;
}

//dr_num = 4 for all 4 hardware breakpoints
bool do_set_dr(PSESSION_INFO pSess, PTHREAD_DATA pThrd, int dr_num)
{
	CONTEXT con;
	dbg_suspend_thread(pThrd->dbg_handle);
	if (!trc_get_thread_ctx(pSess->SessionId, pThrd->trc_thrd.TID, &con))
		return 0;
	if (dr_num == 4)
	{
		for (dr_num=0; dr_num<4; dr_num++)
		{
			(&con.Dr0)[dr_num] = (UINT_PTR)pThrd->breakpoint[dr_num].addr;
			set_dr7(&con.Dr7, dr_num, pThrd->breakpoint[dr_num].type, pThrd->breakpoint[dr_num].range);
		}

	} else 
	{
		(&con.Dr0)[dr_num] = (UINT_PTR)pThrd->breakpoint[dr_num].addr;
		set_dr7(&con.Dr7, dr_num, pThrd->breakpoint[dr_num].type, pThrd->breakpoint[dr_num].range);
	}

	if (!trc_set_thread_ctx(pSess->SessionId, pThrd->trc_thrd.TID, &con))
		return 0;
	dbg_resume_thread(pThrd->dbg_handle);
	return 1;
}

bool set_thrd_hard_bp(PSESSION_INFO pSess, PTHREAD_DATA pThrd, PVOID addr, ULONG type, ULONG range)
{
	int dr_num = add_hard_bp(pThrd, addr, type, range);
	if (dr_num == -1)
		return 0;

	return do_set_dr(pSess, pThrd, dr_num);
}


bool delete_hw(PTHREAD_DATA pThrd, ULONG num)
{
	memset(&pThrd->breakpoint[num], 0, sizeof(pThrd->breakpoint[num]));
	return 1;
}

//4 for all 4 hardware breakpoints
bool do_delete_dr(PSESSION_INFO pSess, PTHREAD_DATA pThrd, ULONG num)
{
	CONTEXT con;
	char sz[256];
	OutputDebugString("do_delete_dr");	
	dbg_suspend_thread(pThrd->dbg_handle);
	if (!trc_get_thread_ctx(pSess->SessionId, pThrd->trc_thrd.TID, &con))
	{
		OutputDebugString("trc_get_thread_ctx returned 0\n");
		return 0;
	}

	OutputDebugString("trc_get_thread_ctx returned 1\n");

	if (num == 4)
	{
		memset(&con.Dr0, 0, 4*sizeof(ULONG));
		con.Dr7 &= 0xFFAA;
	} else 
	{
		(&con.Dr0)[num] = 0;
		con.Dr7 &= ~((15 << (16 + (4*num))) | (1 << (2*num)));
		wsprintf(sz, "dr7 = %x\n", con.Dr7);
		OutputDebugString(sz);
	}

	OutputDebugString("ggg\n");
	if (!trc_set_thread_ctx(pSess->SessionId, pThrd->trc_thrd.TID, &con))
	{
		OutputDebugString("trc_set_thread_ctx returned 0\n");
		return 0;
	}
	dbg_resume_thread(pThrd->dbg_handle);

	
	wsprintf(sz, "dr7 = %x\n", con.Dr7);
	OutputDebugString(sz);
	return 1;
}

bool delete_thrd_hw(PSESSION_INFO pSess, PTHREAD_DATA pThrd, PVOID addr, ULONG type, ULONG range)
{
	char sz[256];
	wsprintf(sz, "delete_thrd_hw(%X, %X, %X, %X, %X)\n", pSess, pThrd, addr, type, range);
	OutputDebugString(sz);

	int i;
	bool res = FALSE;
	for (i=0; i<4; i++)
	{
		if ((pThrd->breakpoint[i].bEnabled)
			&& (pThrd->breakpoint[i].addr == addr)
			&& (pThrd->breakpoint[i].range == range)
			&& (pThrd->breakpoint[i].type & type))
		{
			wsprintf(sz, "cycle in delete_thrd_hw #%X\n", i);
			OutputDebugString(sz);

			delete_hw(pThrd, i);
			do_delete_dr(pSess, pThrd, i);
			res = TRUE;
		}
	}
	return res;
}

bool delete_hard_bp(PSESSION_INFO pSess, ULONG tid, PVOID addr, ULONG type, ULONG range)
{
	if (tid)
	{
		PTHREAD_DATA pThrd = get_thrd_by_tid(pSess, tid);
		return (pThrd) ? delete_thrd_hw(pSess, pThrd, addr, type, range) : 0;
	}

	bool res = FALSE;
	PTHREAD_DATA_EX pThrd = pSess->pThreads;
	while(pThrd)
	{
		if (delete_thrd_hw(pSess, &pThrd->data, addr, type, range))
			res = TRUE;
		pThrd = pThrd->pNext;
	}
	return res;
}
