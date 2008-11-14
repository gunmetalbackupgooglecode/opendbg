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

TRACERAPI ULONG trc_read_memory(ULONG sesId,PVOID addr,PVOID buffer,ULONG size)
{
	char sz[256];
	wsprintf(sz, "trc_read_memory(%X, %X, %X, %X)", sesId, addr, buffer, size);
	//OutputDebugString(sz);

	return read_memory(sesId,addr,buffer,size,TRUE);
}

//------------------------------------------------------------------------
ULONG read_memory(ULONG sesId,PVOID addr,PVOID buffer,ULONG size,BOOLEAN code_page)
{
	PSESSION_INFO CurrSessItem;
	//int result = 0;
	PBREAKPOINT_LIST lpCurrBp;
	//char sz[256];

	//wsprintf(sz, "read_memory(%X, %X, %X, %X, %X)\n", sesId, addr, buffer, size, code_page);
	//OutputDebugString(sz);

	CurrSessItem = get_sess_by_id(sesId);

	if (!CurrSessItem)
		return 0;
	if (!dbg_read_memory(CurrSessItem->pDbgContext, addr, buffer, size, code_page))
		return 0;

	lpCurrBp = CurrSessItem->pSoftBps;
	while (lpCurrBp)
	{
		if (((UINT_PTR)lpCurrBp->addr >= (UINT_PTR)addr) && ((UINT_PTR)lpCurrBp->addr < (size + (UINT_PTR)addr)))
		{
			//wsprintf(sz, "Bp at %X alr. set, delta is %X\n", lpCurrBp->addr, (ULONG)addr - (ULONG)lpCurrBp->addr);
			//OutputDebugString(sz);
			//OutputDebugString("set original byte\n");
			((char*)buffer)[(UINT_PTR)lpCurrBp->addr - (UINT_PTR)addr] = lpCurrBp->SavedByte;
		}
		lpCurrBp = lpCurrBp->pNext;
	}
	return 1;
}

//------------------------------------------------------------------------
TRACERAPI PTHREAD_LIST trc_get_thread_list(ULONG sesId)
{
	PSESSION_INFO CurrSessItem;
	ULONG result, thrd_count;
	PTHREAD_DATA_EX pCurThread, pFirstThread;
	PTHREAD_LIST pThreadList;

	CurrSessItem = get_sess_by_id(sesId);
	if (!CurrSessItem )
		return 0;

	result = 0;

	pCurThread = CurrSessItem->pThreads;
	pFirstThread = CurrSessItem->pThreads;

	thrd_count = 0;
	while (pCurThread)
	{
		pCurThread = pCurThread->pNext;
		thrd_count++;
	}
	if (!thrd_count)
		return 0;

	pThreadList = (PTHREAD_LIST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(TRC_THREAD)*thrd_count + 4);
	pThreadList->count = thrd_count;

	pCurThread = pFirstThread;
	do {
		//char sz[256];
		//wsprintf(sz, " add thread tid:%X, teb:%X", pThreadList->thread[i].thread_id, pThreadList->thread[i].teb_addr);
		//OutputDebugString(sz);

		pThreadList->thread[thrd_count-1].TID = pCurThread->data.trc_thrd.TID;
		pThreadList->thread[thrd_count-1].teb_addr = pCurThread->data.trc_thrd.teb_addr;
		pCurThread = pCurThread->pNext;
		--thrd_count;
	}  while ( thrd_count );
	return pThreadList;
}

//------------------------------------------------------------------------
ULONG write_memory(ULONG sesId, PVOID addr, PVOID buffer, ULONG size, BOOLEAN code_page)
{
	PVOID pBuffer;
	PSESSION_INFO CurrSessItem;
	PBREAKPOINT_LIST lpCurrBp;
	ULONG res = 0;
	char sz[256];
	wsprintf(sz, "write_memory(%X, %X, %X, %X, %X)\n", sesId, addr, buffer, size, code_page);
	OutputDebugString(sz);

	CurrSessItem = get_sess_by_id(sesId);
	if (!CurrSessItem)
		return 0;

	pBuffer = VirtualAlloc(0, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!pBuffer)
		return 0;

	//try
	{
		memcpy(pBuffer, pBuffer, size);

		lpCurrBp = CurrSessItem->pSoftBps;
		if (lpCurrBp)
		do {
			if (((UINT_PTR)lpCurrBp->addr >= (UINT_PTR)addr) && ((UINT_PTR)lpCurrBp->addr < (size + (UINT_PTR)addr)))
			{
				lpCurrBp->SavedByte = ((char*)pBuffer)[(UINT_PTR)lpCurrBp->addr - (UINT_PTR)addr];
				((unsigned char*)pBuffer)[(UINT_PTR)lpCurrBp->addr - (UINT_PTR)addr] = 0xCC;
			}
			lpCurrBp = lpCurrBp->pNext;
		} while (lpCurrBp);
		res = dbg_write_memory(CurrSessItem->pDbgContext, addr, pBuffer, size, code_page);
	}
	/*
	catch(...)
	{
		OutputDebugString("Some exception in trc_write_memory\n");
	}
	*/
	VirtualFree(pBuffer, size, MEM_FREE);

	return res;
}

//------------------------------------------------------------------------
ULONG trc_write_memory(ULONG sesId,PVOID addr,PVOID buffer,ULONG size)
{
	//char sz[256];
	//wsprintf(sz, "trc_write_memory(%X, %X, %X, %X)", sesId, addr, buffer, size);
	//OutputDebugString(sz);

	return write_memory(sesId,addr,buffer,size,TRUE);
}

