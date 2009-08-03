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
#include "utils.h"

__declspec(naked)  DWORD __stdcall htodw(char * lpString)
{
	_asm
	{
		push esi
		push edx
		mov esi, [esp + 0Ch] ; pHexString
	xor edx, edx

ALIGN 4

htodw_loop:     
	mov al, [esi] 
	inc esi
	sub   al, 0x30
	js   htodw_endloop
	shl   edx, 4
	cmp   al, 9
	jbe   Forward
	sub   al,0x27 ;"a" - "0" - 10
	jns   Forward
	add al, 20h

Forward:
	xor dl, al
	jmp htodw_loop

htodw_endloop:
	mov eax, edx

	pop edx
	pop esi

	ret 4
	}
}