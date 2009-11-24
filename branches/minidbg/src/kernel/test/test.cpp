/*
    *
    * Copyright (c) 2008 - 2009, OpenDbg Team
	* ntldr <ntldr@freed0m.org> PGP key ID - 0xC48251EB4F8E4E6E
	* d1mk4 <d1mk4nah@gmail.com>
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
#include <cstdio>
//
//#include "tracer.h"
//#include "breakpoint.h"

int
#ifdef _MSC_VER
__cdecl
#endif
main(int argc, char* argv[])
{
	int** p = new int*[];

	//event_filt filter;
	////HANDLE     pid;
	//dbg_msg    *msg = new dbg_msg;

	//printf("dbgapi test tool started\n");

	//try {
	//	do
	//	{
	//		trc::tracer tracer_test;

	//		printf("dbgapi initialized\n");
	//		printf("dbgapi version as %d\n", dbg_drv_version());

	//		// start tracing
	//		tracer_test.open_process("C:\\Windows\\system32\\notepad.exe");

	//		printf("process started with pid %x\n", tracer_test.get_pid());
	//		printf("debugger attached\n");

	//		filter.event_mask  = DBG_EXCEPTION | DBG_TERMINATED | DBG_START_THREAD | DBG_EXIT_THREAD | DBG_LOAD_DLL;
	//		filter.filtr_count = 0;

	//		if (dbg_set_filter(tracer_test.get_pid(), &filter) == 0) {
	//			printf("dbg_set_filter error\n");
	//			break;
	//		}

	//		printf("debug events filter set up\n");

	//		do
	//		{
	//			u32 continue_status = DBG_CONTINUE;
	//			if (dbg_get_msg_event(tracer_test.get_pid(), msg) == 0) {
	//				printf("get debug message error\n");
	//				break;
	//			}

	//			if (msg->event_code == DBG_TERMINATED)
	//			{
	//				printf("DBG_TERMINATED %x by %x\n",
	//					msg->terminated.proc_id,
	//					msg->process_id
	//					);

	//				continue_status = DBG_CONTINUE;
	//				//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	//			}

	//			if (msg->event_code == DBG_START_THREAD)
	//			{
	//				printf("DBG_START_THREAD %x by %x, teb: %x\n",
	//					msg->thread_start.thread_id,
	//					msg->process_id,
	//					msg->thread_start.teb_addr
	//					);


	//				continue_status = DBG_CONTINUE;
	//				//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	//			}

	//			if (msg->event_code == DBG_EXIT_THREAD)
	//			{
	//				printf("DBG_EXIT_THREAD %x in %x by %x\n",
	//					msg->thread_exit.thread_id,
	//					msg->thread_exit.proc_id,
	//					msg->process_id
	//					);

	//				continue_status = DBG_CONTINUE;
	//				//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	//			}

	//			if (msg->event_code == DBG_EXCEPTION)
	//			{
	//				//printf("DBG_EXCEPTION %0.8x in %x:%x\n",
	//				//	msg->exception.except_record.ExceptionCode,
	//				//	msg->thread_id,
	//				//	msg->process_id
	//				//	);

	//				switch (msg->exception.except_record.ExceptionCode)
	//				{
	//					case EXCEPTION_BREAKPOINT:
	//					{
	//						printf("BREAKPOINT %x\n", msg->exception.except_record.ExceptionAddress);
	//						if ( msg->exception.first_chance )
	//							continue_status = DBG_CONTINUE;
	//						else
	//							continue_status = DBG_EXCEPTION_NOT_HANDLED;
	//					}
	//					break;
	//					
	//					case EXCEPTION_SINGLE_STEP:
	//					{
	//						CONTEXT context;
	//						dbg_get_thread_ctx(msg->thread_id, &context);
	//						printf("SINGLE_STEP %x\n", msg->exception.except_record.ExceptionAddress);
	//						trc::breakpoint bp((u32)msg->process_id, (u32)msg->thread_id, (u3264)msg->exception.except_record.ExceptionAddress);
	//					}
	//					break;

	//				default:
	//					continue_status  = DBG_CONTINUE;
	//					break;
	//				}
	//				//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	//			}

	//			if (msg->event_code == DBG_LOAD_DLL)
	//			{
	//				printf("DBG_LOAD_DLL %ws adr 0x%p sz 0x%x in %x:%x\n",
	//						msg->dll_load.dll_name,
	//						msg->dll_load.dll_image_base,
	//						msg->dll_load.dll_image_size,
	//						msg->thread_id,
	//						msg->process_id
	//						);

	//				continue_status = DBG_CONTINUE;
	//				//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	//			}

	//			if (!tracer_test.enable_single_step(msg->process_id, msg->thread_id))
	//				printf("can't enable single step for %x\n", msg->thread_id);

	//			if (!ContinueDebugEvent((u32)msg->process_id, (u32)msg->thread_id, continue_status))
	//				break;
	//		} while (1);

	//	} while (0);
	//} catch (pdb::pdb_error& e) {
	//	std::cout << e.what() << "\n";
	//}

	////Sleep(INFINITE);

	return 0;
}
