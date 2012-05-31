#include "precompiled.h"

#include "tracer.h"
#include "dbgapi.h"

namespace trc
{

tracer::tracer()
 : m_image_name("")
{
	init(instruction_set_X86);
}

tracer::tracer(instruction_set set)
 : m_image_name("")
{
	init(set);
}

tracer::tracer(instruction_set set, const std::string& image_name)
 : m_image_name(image_name)
{
	init(set);
}

tracer::tracer( const tracer& dbg )
 : m_image_name(dbg.m_image_name),
   m_pid(dbg.m_pid)
{
	m_filter.event_mask = dbg.m_filter.event_mask;
	m_filter.filtr_count = dbg.m_filter.filtr_count;
	for (size_t i = 0; i < dbg.m_filter.filtr_count; ++i)
		m_filter.filters[i] = dbg.m_filter.filters[i];
}

void tracer::init(instruction_set set)
{
	m_pid = 0;
	ud_init(&m_disasm);
	ud_set_mode(&m_disasm, set);
	if (dbg_initialize_api(0x1234, L"c:\\ntoskrnl.pdb", (dbg_sym_get)get_symbols_callback) != 1)
		throw std::exception("dbgapi initialization error");
}

u_long tracer::get_version()
{
	return dbg_drv_version();
}

void tracer::trace_process()
{
	if ((m_pid = dbg_create_process(m_image_name.c_str(), DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS)) == NULL)
		throw std::exception("process not started");
//      printf("process started with pid %x\n", pid);

	ATTACH_INFO attach_info;
	if (dbg_attach_debugger(m_pid, &attach_info) == 0)
		throw std::exception("tracer not attached");
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

	if (msg.event_code == DBG_CREATED)
	{
		std::cout << msg.created.image_base << "\n";
		HANDLE ep_addr = (HANDLE)((u3264)attach_info.image_ep + (u3264)msg.created.image_base);
		try {
			m_bp_trc_array.push_back(breakpoint(reinterpret_cast<u3264>(m_pid),0,reinterpret_cast<u3264>(ep_addr)));
		} catch (tracer_error& e) {
			std::cout << e.what() << "\n";
		}
	}

	if (msg.event_code == DBG_TERMINATED)
	{
		m_terminated_signal(msg);
		continue_status = 0;
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
		switch (msg.exception.except_record.ExceptionCode)
		{
			(msg.exception.first_chance)
				? continue_status = DBG_CONTINUE
				: continue_status = DBG_EXCEPTION_NOT_HANDLED;

		case EXCEPTION_BREAKPOINT:
			for (unsigned int i = 0; i < m_bp_trc_array.size(); ++i)
				if ( m_bp_trc_array[i].get_address() == reinterpret_cast<u3264>(msg.exception.except_record.ExceptionAddress) )
					m_bp_trc_array[i].turn_off();

			m_breakpoint_signal(msg);
			break;

		case EXCEPTION_SINGLE_STEP:
			m_trace_signal(msg);
			break;

		default:
			m_exception_signal(msg);
			break;
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

void tracer::add_breakpoint(u32 proc_id, u32 thread_id, u3264 address)
{
	m_bp_usr_array.push_back(breakpoint(proc_id, thread_id, address));
}

void tracer::del_breakpoint(u32 proc_id, u32 thread_id, u3264 address)
{
	std::remove_if(
		m_bp_usr_array.begin(),
		m_bp_usr_array.end(),
		boost::bind(&breakpoint::get_address, _1) == address
	);
}

bool enable_single_step(HANDLE process_id, HANDLE thread_id)
{
	//u_long readed, cmdlength;
	CONTEXT context;
	context.ContextFlags = CONTEXT_CONTROL;
	if (dbg_get_context(thread_id, &context))
	{
		context.EFlags |= TF_BIT;
		dbg_set_context(thread_id, &context);
		return true;
	}

	return false;
}

bool disable_single_step(HANDLE thread_id)
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

bool tracer::is_untraceable_opcode(u8* opcode)
{
	return (opcode[0] == 0x17) || // pop ss
	(opcode[0] == 0x9D) || // popfd
	(opcode[0] == 0x9C); // pushfd 
}

bool tracer::is_call(u8* opcode)
{
	ud_set_input_buffer(&m_disasm, opcode, MAX_INSTRUCTION_LEN);
	ud_disassemble(&m_disasm);

	return m_disasm.mnemonic == UD_Icall;
}

bool tracer::is_rep(u8* opcode)
{
	ud_set_input_buffer(&m_disasm, opcode, MAX_INSTRUCTION_LEN);
	ud_disassemble(&m_disasm);

	return m_disasm.pfx_rep ||
		m_disasm.pfx_repe ||
		m_disasm.pfx_repne ||
		m_disasm.mnemonic == UD_Irep ||
		m_disasm.mnemonic == UD_Irepne;
}

bool tracer::is_loop(u8* opcode)
{
	ud_set_input_buffer(&m_disasm, opcode, MAX_INSTRUCTION_LEN);
	ud_disassemble(&m_disasm);

	return (m_disasm.mnemonic == UD_Iloop) ||
		(m_disasm.mnemonic == UD_Iloope) ||
		(m_disasm.mnemonic == UD_Iloopnz);
}

void tracer::step_into(u3264 addr)
{
	m_bp_trc_array.push_back(breakpoint(reinterpret_cast<u3264>(m_pid), 0, addr + 1));
}

void tracer::step_over(u3264 addr)
{
	ud_init(&m_disasm);
	ud_set_mode(&m_disasm, 32);
	ud_set_input_buffer(&m_disasm, (u8*)addr, MAX_INSTRUCTION_LEN);
	ud_disassemble(&m_disasm);

	if (m_disasm.mnemonic == UD_Icall) {
		size_t sz = get_size_till_ret((void*)addr);
		m_bp_trc_array.push_back(breakpoint(reinterpret_cast<u3264>(m_pid), 0, addr + sz));
	} else {
		m_bp_trc_array.push_back(breakpoint(reinterpret_cast<u3264>(m_pid), 0, addr + 1));
	}
}

u32 get_size_till_ret(void* fn)
{
	u32 res = 0, len;
	ud_t disasm;
	ud_init(&disasm);
	ud_set_mode(&disasm, 32);

	do
	{
		ud_set_input_buffer(&disasm, (uint8_t*)fn, MAX_INSTRUCTION_LEN);
		ud_disassemble(&disasm);
		len = ud_insn_len(&disasm);
		res += len;
		if ((len == 1) && (disasm.mnemonic == UD_Iret)) break;
		fn = (void*)((u32)fn + len);
	} while (len);

	return res;
}

} // namespace trc

