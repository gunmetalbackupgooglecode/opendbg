#include "stdafx.h"
#include "Debugger.h"
#include "dbgapi.h"

using namespace std;

Debugger::Debugger(void)
{
	msg = new dbg_msg;
	// TODO: remove hardcoded reference
	// TODO: debugger_exception class
	if (dbg_initialize_api(0x1234, L"c:\\ntoskrnl.pdb", (dbg_sym_get)get_symbols_callback) != 1)
		throw exception("dbgapi initialization error");
}

u_long Debugger::GetVersion()
{
	return dbg_drv_version();
}

void Debugger::DebugProcess(string imageName)
{
	auto_ptr<char> cs(const_cast<char*>(imageName.c_str()));
	if ((pid = dbg_create_process(NULL, cs.get(), CREATE_NEW_CONSOLE | DEBUG_ONLY_THIS_PROCESS)) == NULL)
		throw exception("process not started");
//	printf("process started with pid %x\n", pid);

	if (dbg_attach_debugger(NULL, pid) == 0)
		throw exception("debugger not attached");
//	printf("debugger attached\n");
}

void Debugger::Test()
{
	filter.event_mask  = DBG_EXCEPTION | DBG_TERMINATED | DBG_START_THREAD | DBG_EXIT_THREAD | DBG_LOAD_DLL;
	filter.filtr_count = 0;

	if (dbg_set_filter(NULL, pid, &filter) == 0)
		throw exception("dbg_set_filter error");

	printf("debug events filter set up\n");

	do
	{
		u32 continue_status = DBG_CONTINUE;
		if (dbg_get_msg_event(NULL, pid, msg) == 0)
		{
			printf("get debug message error\n");
			break;
		}

		if (msg->event_code == DBG_TERMINATED)
		{
			printf("DBG_TERMINATED %x by %x\n",
				msg->terminated.proc_id,
				msg->process_id
				);

			continue_status = DBG_CONTINUE;
			//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
		}

		if (msg->event_code == DBG_START_THREAD)
		{
			printf("DBG_START_THREAD %x by %x, teb: %x\n",
				msg->thread_start.thread_id,
				msg->process_id,
				msg->thread_start.teb_addr
				);

			continue_status = DBG_CONTINUE;
			//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
		}

		if (msg->event_code == DBG_EXIT_THREAD)
		{
			printf("DBG_EXIT_THREAD %x in %x by %x\n",
				msg->thread_exit.thread_id,
				msg->thread_exit.proc_id,
				msg->process_id
				);

			continue_status = DBG_CONTINUE;
			//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
		}

		if (msg->event_code == DBG_EXCEPTION)
		{
			printf("DBG_EXCEPTION %0.8x in %x:%x\n",
				msg->exception.except_record.ExceptionCode,
				msg->thread_id,
				msg->process_id
				);

			switch (msg->exception.except_record.ExceptionCode)
			{
			case EXCEPTION_BREAKPOINT :
				{
					if ( msg->exception.first_chance )
						continue_status = DBG_CONTINUE ;
					else
						continue_status = DBG_EXCEPTION_NOT_HANDLED ;
				}
				break ;

			default:
				continue_status  = DBG_CONTINUE ;
				break ;
			}
			//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
		}

		if (msg->event_code == DBG_LOAD_DLL)
		{
			printf("DBG_LOAD_DLL %ws adr 0x%p sz 0x%x in %x:%x\n",
				msg->dll_load.dll_name,
				msg->dll_load.dll_image_base,
				msg->dll_load.dll_image_size,
				msg->thread_id,
				msg->process_id
				);

			continue_status = DBG_CONTINUE;
			//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
		}

		if (!ContinueDebugEvent((u32)msg->process_id, (u32)msg->thread_id, continue_status))
			break;
	} while (1);
}

// modified version for xp sp3
uintptr_t
CALLBACK Debugger::get_symbols_callback(
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

Debugger::~Debugger(void)
{
	delete msg;
}
