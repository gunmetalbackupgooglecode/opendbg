/*
    *    
	* Copyright (c) 2009
	* vol4ok <vol4ok@highsecure.ru> PGP key ID - 0x7A1C8BB4A0F34B67
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
#include "extdef.h"
#include "misc.h"

void set_sdt_hook(int num, void *np, void *op)
{
	u64 cr0;

	cr0          =  mem_open();
	ppv(op)[0]   = SYSCALL(num);
	SYSCALL(num) = np;
	mem_close(cr0);
}