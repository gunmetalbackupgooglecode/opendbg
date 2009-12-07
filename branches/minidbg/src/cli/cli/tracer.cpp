#include "stdafx.h"
#include "tracer.h"
#include "dbgapi.h"

using namespace std;

tracer::tracer()
 : m_image_name("")
{
	// TODO: remove hardcoded reference
	// TODO: debugger_exception class
	init();
}

tracer::tracer(const std::string& image_name)
 : m_image_name(image_name)
{
	init();
}

void tracer::init()
{
	m_pid = 0;
	if (dbg_initialize_api(0x1234, L"c:\\ntoskrnl.pdb", (dbg_sym_get)get_symbols_callback) != 1)
		throw std::exception("dbgapi initialization error");
}

u_long tracer::get_version()
{
	return dbg_drv_version();
}

void tracer::trace_process()
{
	if ((m_pid = dbg_create_process(m_image_name.c_str(), CREATE_NEW_CONSOLE | DEBUG_ONLY_THIS_PROCESS)) == NULL)
		throw std::exception("process not started");
//      printf("process started with pid %x\n", pid);

//  ISSUE: it's an invalid representation about
//  how debugger attaches to processes;
//  dbg_create_process and dbg_attach_debugger
//  different ways to start debugging
//
//	if (dbg_attach_debugger(m_pid) == 0)
//		throw std::exception("tracer not attached");
//      printf("tracer attached\n");

	m_filter.event_mask  = DBG_EXCEPTION | DBG_TERMINATED | DBG_START_THREAD | DBG_EXIT_THREAD | DBG_LOAD_DLL;
	m_filter.filtr_count = 0;

	if (dbg_set_filter(m_pid, &m_filter) == 0)
		throw std::exception("dbg_set_filter error");

	printf("debug events filter set up\n");

	do
	{
	dbg_msg msg;
	u32 continue_status = DBG_CONTINUE;
	if (dbg_get_msg_event(m_pid, &msg) == 0)
	{
		printf("get debug message error\n");
		break;
	}

	if (msg.event_code == DBG_TERMINATED)
	{
		m_terminated_signal(msg);
		continue_status = DBG_CONTINUE;
		//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	}

	if (msg.event_code == DBG_START_THREAD)
	{
		m_start_thread_signal(msg);
		continue_status = DBG_CONTINUE;
		//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	}

	if (msg.event_code == DBG_EXIT_THREAD)
	{
		m_exit_thread_signal(msg);
		continue_status = DBG_CONTINUE;
		//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	}

	if (msg.event_code == DBG_EXCEPTION)
	{
		m_exception_signal(msg);

		switch (msg.exception.except_record.ExceptionCode)
		{
		case EXCEPTION_BREAKPOINT :
			{
				m_breakpoint_signal(msg);
				if ( msg.exception.first_chance )
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

	if (msg.event_code == DBG_LOAD_DLL)
	{
		m_dll_load_signal(msg);
		continue_status = DBG_CONTINUE;
		//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	}

	if (!ContinueDebugEvent((u32)msg.process_id, (u32)msg.thread_id, continue_status))
		break;
	} while (1);
}

// modified version for xp sp3
uintptr_t
CALLBACK tracer::get_symbols_callback(int sym_type, char * sym_name, char * sym_subname, pdb::pdb_parser& pdb)
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

bool tracer::enable_single_step(HANDLE process_id, HANDLE thread_id)
{
	u_long readed, cmdlength;
	CONTEXT context;
	context.ContextFlags = CONTEXT_CONTROL;
	if (dbg_get_context(thread_id, &context))
	{
		context.EFlags |= TF_BIT;
		dbg_set_context(thread_id, &context);
		//// проверяем возможность трейса команды
		//if (!dbg_read_memory(process_id, (char*)context.Eip, &buf, sizeof(buf), &readed))
		//	return false;
		//cmdlength = disassemble(buf, this->instr, &this->params);
		//if (analyser::is_instruction_untraceable(*this->instr)) {
		//	breakpoint bp((u32)process_id, (u32)thread_id, (u3264)(context.Eip + cmdlength));// поставить int 3 бряк
		//}
		//// записать что это quick-бряк

		return true;
	}

	return false;
}

bool tracer::disable_single_step(HANDLE thread_id)
{
	CONTEXT context;
	if (dbg_get_context(thread_id, &context))
	{
		context.EFlags &= ~TF_BIT;
		dbg_set_context(thread_id, &context);
		return true;
	}

	return false;
}

void tracer::add_breakpoint(u32 proc_id, u32 thread_id, u3264 address)
{
	m_bp_array.push_back(breakpoint(proc_id, thread_id, address));
}

void tracer::del_breakpoint(u32 proc_id, u32 thread_id, u3264 address)
{
	std::remove_if(
		m_bp_array.begin(),
		m_bp_array.end(),
		boost::bind(&breakpoint::get_address, _1) == address
	);
}

