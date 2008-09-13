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

#include "LDasm.h"
#include "trc_types.h"

#define IS_CALL_IN_BUFFER(x)  (( *(UCHAR*)(x) == 0xE8) || ( *(UCHAR*)(x) == 0x9A) || (( *(UCHAR*)(x) == 0xFF) & (( ((UCHAR*)(x))[1] & 0x38) == 0x10))) // call near/far "rm/32")


bool IsChainCmd(PUCHAR buff)
{
	static const char cmds[] = {0x6E, 0x6C, 0xEE, 0xEC,  0xAC, 0xA6, 0xA4, 0xAA, 0xAE};
	if ((buff[0] & 0xFE) != 0xF2)
		return 0;

	int i;
	for (i=0; i<sizeof(cmds); i++)
	{
		if ((buff[1] & 0xFE) == cmds[i])
		{
			char sz[256];
			wsprintf(sz, "Chain cmd - %.2X %.2X\n", buff[0], buff[1]);
			OutputDebugString(sz);
			return 1;
		}
	}

	return 0;
}

bool IsTraceOverCmd(PSESSION_INFO pSess, PVOID addr)
{
	UCHAR buff[16];

	OutputDebugString("======IsCallAtAddr======\n");
	trc_read_memory(pSess->SessionId, addr, &buff, 16);

	return ((IS_CALL_IN_BUFFER(buff)) || (IsChainCmd(buff)));
}


bool IsUntraceableCmd(PUCHAR pBuffer)
{	
	if (pBuffer[0] == 0x17) //pop ss
		return TRUE;
	if (pBuffer[0] == 0x9D) //popfd
		return TRUE;
	if (pBuffer[0] == 0x9C) //pushfd
		return TRUE;
	return FALSE;
}




//----- (10004BC0) --------------------------------------------------------
DWORD get_ep_rva(PVOID pModBase)
{	
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pModBase;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + (DWORD)pDos);

	return pNt->OptionalHeader.AddressOfEntryPoint;	
}

DWORD get_tls_dir(PVOID pModBase, ULONG num)
{
	return *(DWORD *)(*(DWORD *)((ULONG)pModBase + 60) + 8 * num + 120 + (ULONG)pModBase);
}


int Get_code_size(PSESSION_INFO pSess, PVOID addr)
{
	UCHAR buff[16];

	OutputDebugString("======Get_code_size======\n");
	trc_read_memory(pSess->SessionId, addr, &buff, 16);
	return SizeOfCode(buff, NULL);
}
