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
#include "syscall.h"
#include "regio.h"
#include "extdef.h"
#include "sdt_hook.h"
#include "misc.h"
#include "driver.h"
#include "debug.h"

static PZWTERMINATEPROCESS old_zw_term_proc;
static u32                 sc_key; /* syscall access key */

static scproc syscall_table[] = {
	dbg_get_version,       // 0x00
	dbg_open_process,      // 0x01
	dbg_terminate_process, // 0x02
	dbg_attach_call,       // 0x03
	dbg_get_message,       // 0x04
	dbg_set_filter,        // 0x05
	dbg_continue          // 0x06
};

#define SC_MAX       (sizeof(syscall_table)/sizeof(syscall_table[0]))
#define SC_DAT_LIMIT (1024*1024)

static
NTSTATUS
  new_zw_term_proc(
    HANDLE   h_process,
    NTSTATUS exit_status
    )
{
	NTSTATUS status;
	buf_lock in_lock;
	buf_lock out_lock;	
	syscall  call;
	
	if ( (ExGetPreviousMode() == UserMode) && (exit_status == sc_key) ) 
	{
		status = STATUS_UNSUCCESSFUL;
		in_lock.data = NULL; out_lock.data = NULL;

		do
		{
			__try
			{
				ProbeForRead(h_process, sizeof(syscall), 1);
				
				/* capture syscall data */
				fastcpy(&call, h_process, sizeof(syscall));
			}
			__except(EXCEPTION_EXECUTE_HANDLER) 
			{
				status = GetExceptionCode();
				break;			
			}

			if ( (call.number   >= SC_MAX)   || 
				 (call.in_size  > SC_DAT_LIMIT) ||
				 (call.out_size > SC_DAT_LIMIT) ) 
			{				
				break;
			}

			/* lock syscall buffers */
			if (call.in_data != NULL) 
			{
				if (dbg_lock_um_buff(&in_lock, call.in_data, call.in_size) == 0) {
					break;
				}

				call.in_data = in_lock.data;
			}

			if (call.out_data != NULL)
			{
				if (dbg_lock_um_buff(&out_lock, call.out_data, call.out_size) == 0) {
					break;
				}

				call.out_data = out_lock.data;
			}

			/* execute syscall handler */
			if (syscall_table[call.number](&call) != 0) {
				status = STATUS_SUCCESS;
			}
		} while (0);

		if (in_lock.data != NULL) {
			dbg_unlock_um_buff(&in_lock);
		}

		if (out_lock.data != NULL) {
			dbg_unlock_um_buff(&out_lock);
		}
	} else 
	{
		status = old_zw_term_proc(
			h_process, exit_status
			);
	}

	return status;
}


int init_syscall(HANDLE h_key)
{
	u32 num;
	int succs = 0;

	do
	{
		/* get index of ZwTerminateProcess */
		if (reg_query_val(h_key, L"sym_ZwTerminateProcess", &num, sizeof(num)) == 0) {
			break;
		}

		/* get syscall key */
		if (reg_query_val(h_key, L"sc_key", &sc_key, sizeof(sc_key)) == 0) {
			break;
		}

		set_sdt_hook(
			num, new_zw_term_proc, &old_zw_term_proc
			);

		DbgMsg("num %d, sc_key %x\n", num, sc_key);

		succs = 1;
	} while (0);

	return succs;
}