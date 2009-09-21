/*
 *
 * Copyright (c) 2009
 * Vladimir <progopis@jabber.ru> PGP key ID - 0x59CF2D8A75CB8417
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
#include <windows.h>
#include <vector>
#include "breakpoints.h"
#include "dbgapi.h"
#include "test.h"

int trc::breakpoint::set_breakpoint (u32 proc_id, u32 thread_id, u3264 address, brk_type break_type)
{
	BYTE buf;
	u32 readed;
	// ¬ случае установки soft-брикпойнта (int 3) проверить
	// чтобы его не было там изначально, а если есть --
	// добавить флаг дл€ генерации исключени€ после срабатывани€
	switch (break_type)
	{
	case BRK_TYPE_INT3:
		if (!dbg_read_memory((HANDLE)proc_id, (PVOID)address, (PVOID)&buf, sizeof(buf), &readed))
			return 1; // пам€ть не может быть прочитана
		
		break;
	case BRK_TYPE_HWR:
	case BRK_TYPE_HWE:
	case BRK_TYPE_HWA:
		break;
	}
	return 0;
}

int trc::breakpoint::delete_breakpoint (u32 proc_id, u32 thread_id, u3264 address)
{
	return 0;
}

int trc::breakpoint::is_breakpoint_exists (u32 proc_id, u32 thread_id, u3264 address)
{	
	return 0;
}
