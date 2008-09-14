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


//sub_10001D30 - set modules?
ULONG create_modules(PSESSION_INFO pSessData, PMODULE_INFO_EX pModuleInfoEx)
{	
	DLL_LOAD_INFO tmp;

	for (ULONG i=0; i<pModuleInfoEx->mod_count ; i++)
	{
		lstrcpyn(tmp.file_name, pModuleInfoEx->modules[i].mod_file, 0x104);
		tmp.image_size = pModuleInfoEx->modules[i].mod_size;
		tmp.image_base = pModuleInfoEx->modules[i].mod_base;
		add_module(pSessData, &tmp);
	}
	return 0;
}


//sub_10001C70 - add module?
MODULE_LIST* __cdecl add_module(PSESSION_INFO pSessData, PDLL_LOAD_INFO pSmth)
{
	
	MODULE_LIST* pFirst, *pMem;
	char str[256];
	
	wsprintf(str, "add_module(%X, %X)", pSessData, pSmth);
	//OutputDebugString(str);
	pMem = (PMODULE_LIST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(MODULE_LIST)); 
	if (pMem == NULL)
		return 0;

	pFirst = pSessData->pModules;
	pMem->data.ImageBase = pSmth->image_base; // mod_base
	pMem->data.ImageSize = pSmth->image_size; // mod_size

	lstrcpyn(pMem->data.ModName, pSmth->file_name, sizeof(pMem->data.ModName)-1);
	//OutputDebugString("module added======\n");
	wsprintf(str, "imagebase:%#x,size:%#x\n", pMem->data.ImageBase , pMem->data.ImageSize);
	OutputDebugString(pMem->data.ModName);
	OutputDebugString(str);
	OutputDebugString("\n==================\n");
	
	pMem->pNext = pSessData->pModules;
	pSessData->pModules = pMem;

	return pMem;
}
/**/


//----- (10001E50) --------------------------------------------------------
PMODULE_LIST __cdecl get_module_by_base(PSESSION_INFO pSessData, PVOID image_base)
{
	PMODULE_LIST pCurModule; // eax@1
	
	pCurModule = pSessData->pModules;
	while (pCurModule)
	{		
		if (pCurModule->data.ImageBase == image_base)
		{
			return pCurModule;			
		}
		pCurModule = pCurModule->pNext;
	}

	return NULL;
}

//----- (10001DA0) --------------------------------------------------------
signed long __cdecl delete_module(PSESSION_INFO pSessData, PDLL_UNLOAD_INFO unload_info)
{	
	PMODULE_LIST pCurMod; // esi@1	
	PMODULE_LIST *pPrev; // ecx@5
//	CHAR OutputString[256]; // [sp+4h] [bp-40h]@8

	pCurMod = pSessData->pModules;
	pPrev = &pSessData->pModules;
	while ( pCurMod )
	{		
		if (pCurMod->data.ImageBase == unload_info->image_base)
			break;
		pPrev = &pCurMod->pNext;
		pCurMod = pCurMod->pNext;
	}		

	if (!pCurMod)
		return 0;

	*pPrev = pCurMod->pNext;
	HeapFree(hHeap, 0, pCurMod);
	return 1;
}

void delete_all_modules(PSESSION_INFO pSess)
{
	PMODULE_LIST pOld, pCur = pSess->pModules;
	
	while (pCur)
	{
		pOld = pCur;
		pCur = pOld->pNext;
		HeapFree(hHeap, 0, pOld);	
	} 

	pSess->pModules = NULL;
}

//------------------------------------------------------------------------
TRACERAPI PMOD_LIST trc_get_module_list(ULONG sesId)
{
	PSESSION_INFO CurrSessItem;
	ULONG mod_count;
	PMODULE_LIST pFirstModule, pCurModule;
	PMOD_LIST pModList;
	
	CurrSessItem = get_sess_by_id(sesId);
	if (!CurrSessItem)
		return 0;
	
	pCurModule = pFirstModule = CurrSessItem->pModules; //v5 = *(_DWORD *)(v1 + 44);	
	
	mod_count = 0;
	while (pCurModule)
	{		
		++mod_count;
		pCurModule = pCurModule->pNext;
	}
	if (!mod_count)
		return 0;	

	pModList = (PMOD_LIST)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(TRC_MODULE)*mod_count + 4);
	
	char sz[256];;
	wsprintf(sz, "Create list with %x length: %x\n", sizeof(TRC_MODULE)*mod_count + 4, pModList);
	OutputDebugString(sz);

	if (pModList) {
		pCurModule = CurrSessItem->pModules;
		pModList->count = mod_count;

		while (mod_count)
		{
			memcpy(&(pModList->module[mod_count-1]), &pCurModule->data, sizeof(TRC_MODULE));
			pCurModule = pCurModule->pNext;
			mod_count--;
		}
	}
	return pModList;	
}