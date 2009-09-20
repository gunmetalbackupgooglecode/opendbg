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
#include <stdio.h>
#include "defines.h"
#ifndef _BREAKS_
#define _BREAKS_

namespace trc
{

enum brk_type { NO_BREAK, BRK_TYPE_INT3, BRK_TYPE_HWR, BRK_TYPE_HWE, BRK_TYPE_HWA };

typedef struct _BREAK_POINT
{
	u3264 addr;
	u8 soft_orig_byte;
	brk_type type;
} BREAK_POINT;

class breakpoint
{
	public:
		int set_breakpoint (u32 proc_id, u32 thread_id, u3264 address, brk_type break_type);
		int delete_breakpoint (u32 proc_id, u32 thread_id, u3264 address);
		int is_breakpoint_exists (u32 proc_id, u32 thread_id, u3264 address);
	private:
		std::vector <BREAK_POINT> breaks;
		std::vector <BREAK_POINT>::iterator result;
};

}

#endif
