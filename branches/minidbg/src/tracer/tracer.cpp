#include "stdafx.h"

#include "tracer.h"
#include "Process.h"

#pragma intrinsic(_ReadWriteBarrier)


namespace debugger{

tracer::tracer()
 : m_image_name("")
{
	init(X86);
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
	u3264 ep_addr;
	u32 continue_status = DBG_CONTINUE;
	if (dbg_get_msg_event(m_pid, &msg) == 0)
	{
		printf("get debug message error\n");
		break;
	}

	m_tid = msg.thread_id;

	if (msg.event_code == DBG_CREATED)
	{
		sys::PROCESS_PTR ps(new sys::Process((u3264)m_pid));
		process = ps;
		sys::THREAD_PTR targetThread(new sys::Thread((u3264)m_tid));
		std::cout << msg.created.image_base << "\n";
		ep_addr = (u3264)attach_info.image_ep + (u3264)msg.created.image_base;
		
		try {
			// @todo replace with temprorary breakpoint
			debugger::BREAKPOINT_PTR breakpoint(new debugger::SoftwareBreakpoint(process, targetThread,
				ep_addr, 1, debugger::Breakpoint::onExecute, false));
			breakpointsManager.addBreakpoint(breakpoint);
		} catch (DebuggerException& e) {
			std::cout << e.what() << "\n";
		}
		m_created_signal(msg);
		continue_status = DBG_CONTINUE;
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
			{
				u3264 mem_addr = reinterpret_cast<u3264>(msg.exception.except_record.ExceptionAddress);
				BREAKPOINT_PTR stop = breakpointsManager.findBreakpoint(mem_addr);
				if (stop.get()) {
					stop.get()->disable();
					// jump back to exception address
					CONTEXT dbg_context;
					dbg_context.ContextFlags = CONTEXT_FULL;
					dbg_get_context((HANDLE)m_tid, &dbg_context);
					dbg_context.Eip--;
					dbg_set_context((HANDLE)m_tid, &dbg_context);
					// delete temprorary breakpoints
					if (stop.get()->get_type() == brk_temp)
						breakpointsManager.deleteBreakpoint(mem_addr);
					// generate Breakpoint signal
                    m_breakpoint_signal(msg);
				}
			}
			// @todo process unknown breakpoints

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

	if (msg.event_code == DBG_TERMINATED)
	{
		m_terminated_signal(msg);
		continue_status = 0;
		//dbg_continue_event(NULL, pid, RES_NOT_HANDLED, NULL);
	}

	if (!ContinueDebugEvent((u32)msg.process_id, (u32)msg.thread_id, continue_status))
		break;
	} while (1);
}

void tracer::stop_process()
{
	dbg_terminate_process(m_pid);
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
/*
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
}*/

bool enable_single_step(HANDLE process_id, HANDLE thread_id)
{
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_CONTROL;
	if (dbg_get_context(thread_id, &ctx))
	{
		ctx.EFlags |= TF_BIT;
		dbg_set_context(thread_id, &ctx);
		return true;
	}

	return false;
}

bool disable_single_step(HANDLE thread_id)
{
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_CONTROL;
	if (dbg_get_context(thread_id, &ctx))
	{
		ctx.EFlags &= ~TF_BIT;
		dbg_set_context(thread_id, &ctx);
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

bool tracer::is_call()
{
	return m_disasm.mnemonic == UD_Icall;
}

bool tracer::is_rep()
{
	return m_disasm.pfx_rep ||
		m_disasm.pfx_repe ||
		m_disasm.pfx_repne ||
		m_disasm.mnemonic == UD_Irep ||
		m_disasm.mnemonic == UD_Irepne;
}

bool tracer::is_loop()
{
	return (m_disasm.mnemonic == UD_Iloop) ||
		(m_disasm.mnemonic == UD_Iloope) ||
		(m_disasm.mnemonic == UD_Iloopnz);
}

void tracer::step_into()
{
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_FULL;
	dbg_get_context(m_tid, &ctx);

	
	u8 buf[MAX_INSTRUCTION_LEN];
	dbg_read_memory(m_pid, (void*)ctx.Eip, &buf, MAX_INSTRUCTION_LEN*sizeof(u8), NULL);

	if (is_untraceable_opcode(buf))
	{
		ud_init(&m_disasm);
		ud_set_mode(&m_disasm, 32);
		
		ud_set_syntax(&m_disasm, UD_SYN_INTEL);
		ud_set_input_buffer(&m_disasm, buf, MAX_INSTRUCTION_LEN);
		size_t sz = ud_disassemble(&m_disasm);
		// @todo fix this
		sys::THREAD_PTR targetThread(new sys::Thread((u3264)m_tid));
		// @todo replace with temprorary breakpoint
		debugger::BREAKPOINT_PTR breakpoint(new debugger::SoftwareBreakpoint(process, targetThread,
			ctx.Eip + sz, 1, debugger::Breakpoint::onExecute, false));
		breakpointsManager.addBreakpoint(breakpoint);
		//m_bp_trc_array.push_back(breakpoint(reinterpret_cast<u3264>(m_pid), 0, ctx.Eip + sz));
	} else {
		enable_single_step(m_pid, m_tid);
	}
}

void tracer::step_over()
{
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_FULL;
	dbg_get_context(m_tid, &ctx);

	ud_init(&m_disasm);
	ud_set_mode(&m_disasm, 32);

	u8 buf[MAX_INSTRUCTION_LEN];
	dbg_read_memory(m_pid, (void*)ctx.Eip, &buf, MAX_INSTRUCTION_LEN*sizeof(u8), NULL);
	ud_set_syntax(&m_disasm, UD_SYN_INTEL);
	ud_set_input_buffer(&m_disasm, buf, MAX_INSTRUCTION_LEN);
	size_t sz = ud_disassemble(&m_disasm);

	if (is_loop() || is_rep() || is_call()) {
		//size_t sz = get_size_till_ret((void*)ctx.Eip);
		//m_bp_trc_array.push_back(breakpoint(reinterpret_cast<u3264>(m_pid), 0, ctx.Eip + sz));
	} else {
		enable_single_step(m_pid, m_tid);
	}
}

void tracer::step_out()
{
	return; // @todo rewrite it from scratch!
	CONTEXT ctx;
	dbg_get_context(m_tid, &ctx);

	size_t sz = get_size_till_ret((void*)ctx.Eip); // wrong!
	//m_bp_trc_array.push_back(breakpoint(reinterpret_cast<u3264>(m_pid), 0, ctx.Eip + sz));
}

u32 get_size_till_ret(void* fn)
{
	u32 res = 0, len;
	ud_t disasm;
	ud_init(&disasm);
	ud_set_mode(&disasm, 32);

	// @todo fixme (need analyzer)
	do
	{
		ud_set_input_buffer(&disasm, (uint8_t*)fn, MAX_INSTRUCTION_LEN);
		ud_disassemble(&disasm);
		len = ud_insn_len(&disasm);
		res += len;
		if ((disasm.mnemonic == UD_Iretf) || (disasm.mnemonic == UD_Iret)) break;
		fn = (void*)((u32)fn + len);
	} while (len);

	return res;
}

} // namespace trc

