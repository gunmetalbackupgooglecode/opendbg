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
#include "regio.h"
#include "syscall.h"
#include "driver.h"
#include "misc.h"
#include "dbg_item.h"
#include "debug.h"
#include "pedef.h"

void *ntkrn_base;

int dbg_get_version(syscall *data)
{
	if (data->out_size == sizeof(u32))
	{
		p32(data->out_data)[0] = DBG_VERSION;

		return 1;
	}

	return 0;
}

NTSTATUS DriverEntry(
			IN PDRIVER_OBJECT  drv_obj,
            IN PUNICODE_STRING reg_pth
			)
{
//	PIMAGE_DOS_HEADER d_head;
//	PIMAGE_NT_HEADERS n_head;
	NTSTATUS          status = STATUS_UNSUCCESSFUL;
	HANDLE            h_key  = NULL;
//	u32               tstamp;

	do
	{
		if ( (h_key = reg_open_key(reg_pth->Buffer, NULL)) == NULL ) {
			DbgMsg("can not open service key\n");
			break;
		}

		/* get ntoskrnl base */
		if ( (ntkrn_base = get_kernel_base()) == NULL ) {
			DbgMsg("kernel base not found\n");
			break;
		}

		DbgMsg("kernel base: %x\n", ntkrn_base);

		/* get ntoskrnl timestamp */
		/* turned off because of the futility: dbghelp do this stuf
		if (reg_query_val(h_key, L"sym_timestamp", &tstamp, sizeof(tstamp)) == 0) {
			DbgMsg("unable to get ntoskrnl timestamp\n");
			break;
		}
		*/
		/* validate ntoskrnl timestamp */
		/*
		d_head = ntkrn_base;
		n_head = addof(d_head, d_head->e_lfanew);

		if (n_head->FileHeader.TimeDateStamp != tstamp) {
			DbgMsg("invalid ntoskrnl timestamp\n");
			break;
		}
		*/

		if (init_dbg_item(h_key) == 0) {
			DbgMsg("init_dbg_item failed\n");
			break;
		}

		if (init_debug(h_key) == 0) {
			DbgMsg("init_debug failed\n");
			break;
		}

		if (init_syscall(h_key) == 0) {
			DbgMsg("init_syscall failed\n");
			break;
		}

		DbgMsg("dbgapi initialized\n");
		status = STATUS_SUCCESS;
	} while (0);

	if (h_key != NULL) {
		ZwClose(h_key);
	}

	return status;
}

