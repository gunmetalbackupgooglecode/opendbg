/*
    *
    * Copyright (c) 2008
    * ntldr <ntldr@freed0m.org> PGP key ID - 0xC48251EB4F8E4E6E
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

#include <ntifs.h>
#include "defines.h"
#include "misc.h"
#include "code_hook.h"
#include "ldasm.h"

void *hook_code(void *src_proc, void *new_proc)
{
	u8  *src_addr;
	u8  *dst_addr;
	u8  *old_proc;
	u32  cmd_len, flags;
	u32  all_len  = 0;
	u8  *opcd;
	u64  cr0;

	if ( (dst_addr = mem_alloc(100)) == NULL ) {
		return NULL;
	}

	old_proc = dst_addr; src_addr = src_proc;

	do
	{
		cmd_len = size_of_code(src_addr, &opcd);
		flags   = x_code_flags(src_addr);

		if (cmd_len == 0 || flags & OP_REL8)
		{
			return NULL;
		}

		memcpy(dst_addr, src_addr, cmd_len);

		if (flags & OP_REL32)
		{
			if (flags & OP_EXTENDED) {
				p32(dst_addr+2)[0] += src_addr - dst_addr;
			} else {
				p32(dst_addr+1)[0] += src_addr - dst_addr;
			}
		}

		src_addr += cmd_len;
		dst_addr += cmd_len;
		all_len  += cmd_len;
	} while (all_len < 5);

	dst_addr[0]            = 0xE9;
	p32(dst_addr+1)[0]     = src_addr - dst_addr - 5;
	src_addr = src_proc; dst_addr = new_proc;

	cr0 = mem_open();
	src_addr[0]        = 0xE9;
	p32(src_addr+1)[0] = dst_addr - src_addr - 5;
	mem_close(cr0);

	return old_proc;
}
