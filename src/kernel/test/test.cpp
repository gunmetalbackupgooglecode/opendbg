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

#include <windows.h>
#include <stdio.h>

#include "test.h"
#include "dbgapi.h"

// modified version for xp sp3
static uintptr_t
CALLBACK get_symbols_callback(
		int sym_type, char * sym_name, char * sym_subname, pdb::pdb_parser& pdb
		)
{

	if (sym_type == SYM_TIMESTAMP)
	{
		return 0x45E53F9C;
	}

	if (sym_type == SYM_NTAPI_NUM)
	{
		if (strcmp(sym_name, "ZwTerminateProcess") == 0) {
			return 0x101;
		}

		if (strcmp(sym_name, "ZwCreateThread") == 0) {
			return 0x035;
		}

		if (strcmp(sym_name, "ZwTerminateThread") == 0) {
			return 0x102;
		}
	}

	wchar_t sym_name_w[255];
	MultiByteToWideChar(
		CP_ACP, 0, sym_name, strlen(sym_name)+1,
		sym_name_w, sizeof(sym_name_w)/sizeof(sym_name_w[0])
	);

	wchar_t sym_subname_w[255];
	if (sym_subname && strlen(sym_subname)) {
		MultiByteToWideChar(
			CP_ACP, 0, sym_subname, strlen(sym_subname)+1,
			sym_subname_w, sizeof(sym_subname_w)/sizeof(sym_subname_w[0])
		);
	}

	if (sym_type == SYM_OFFSET) {
		return pdb.get_symbol(sym_name_w).get_rva();
	}

	if (sym_type == SYM_STRUCT_OFFSET)
		return pdb.get_type(sym_name_w).get_member(sym_subname_w).get_offset();

	return 0;
}

int main(int argc, char* argv[])
{
	event_filt filter;
	HANDLE     pid;
	dbg_msg    msg;

	printf("dbgapi test tool started\n");

	try {
		do
		{
			if (dbg_initialize_api(0x1234, L"ntkrnlpa.pdb", (dbg_sym_get)get_symbols_callback) == 0) {
				printf("dbgapi initialization error\n");
				break;
			}

			printf("dbgapi initialized\n");
			printf("dbgapi version as %d\n", dbg_drv_version());

			if ( (pid = dbg_create_process(NULL, "\"C:\\Windows\\system32\\calc.exe\"", 0)) == NULL) {
				printf("process not started\n");
				break;
			}

			printf("process started with pid %x\n", pid);

			if (dbg_attach_debugger(NULL, pid) == 0) {
				printf("debugger not attached\n");
				break;
			}

			printf("debugger attached\n");

			filter.event_mask  = DBG_EXCEPTION | DBG_TERMINATED | DBG_START_THREAD | DBG_EXIT_THREAD | DBG_LOAD_DLL;
			filter.filtr_count = 0;

			if (dbg_set_filter(NULL, pid, &filter) == 0) {
				printf("dbg_set_filter error\n");
				break;
			}

			printf("debug events filter setuped\n");

			do
			{
				if (dbg_get_msg_event(NULL, pid, &msg) == 0) {
					printf("get debug message error\n");
					break;
				}

				if (msg.event_code == DBG_TERMINATED)
				{
					printf("DBG_TERMINATED %x by %x\n",
						msg.terminated.proc_id,
						msg.process_id
						);

					dbg_countinue_event(NULL, pid, RES_NOT_HANDLED, NULL);
				}

				if (msg.event_code == DBG_START_THREAD)
				{
					printf("DBG_START_THREAD %x by %x, teb: %x\n",
						msg.thread_start.thread_id,
						msg.process_id,
						msg.thread_start.teb_addr
						);

					dbg_countinue_event(NULL, pid, RES_NOT_HANDLED, NULL);
				}

				if (msg.event_code == DBG_EXIT_THREAD)
				{
					printf("DBG_EXIT_THREAD %x in %x by %x\n",
						msg.thread_exit.thread_id,
						msg.thread_exit.proc_id,
						msg.process_id
						);

					dbg_countinue_event(NULL, pid, RES_NOT_HANDLED, NULL);
				}

				if (msg.event_code == DBG_EXCEPTION)
				{
					printf("DBG_EXCEPTION %0.8x in %x:%x\n",
						msg.exception.except_record.ExceptionCode,
						msg.thread_id,
						msg.process_id
						);

					dbg_countinue_event(NULL, pid, RES_NOT_HANDLED, NULL);
				}
				
				if (msg.event_code == DBG_LOAD_DLL)
				{
					printf("DBG_LOAD_DLL %ws adr 0x%p sz 0x%x in %x:%x\n",
							 msg.dll_load.dll_name,
							 msg.dll_load.dll_image_base,
							 msg.dll_load.dll_image_size,
							 msg.thread_id,
							 msg.process_id
							 );
					
					dbg_countinue_event(NULL, pid, RES_NOT_HANDLED, NULL);
				}

			} while (1);

		} while (0);
	} catch (pdb::pdb_error& e) {
		std::cout << e.what() << "\n";
	}

	Sleep(INFINITE);

	return 0;
}
