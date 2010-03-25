#include "stdafx.h"
#include "logwindow.h"

#include "dbg_def.h"
#include "dbgapi.h"
#include "udis86.h"

const size_t max_instruction_len = 16;

void log_window::created_slot(dbg_msg msg)
{
	append(tr("started debugger"));
}

void log_window::debug_slot(dbg_msg msg)
{
	ud_t disasm;
	ud_init(&disasm);
	ud_set_mode(&disasm, 32);
	u8 buf[max_instruction_len];
	dbg_read_memory(msg.process_id, msg.exception.except_record.ExceptionAddress, &buf, max_instruction_len*sizeof(u8), NULL);
	ud_set_syntax(&disasm, UD_SYN_INTEL);
	ud_set_input_buffer(&disasm, buf, max_instruction_len);
	ud_disassemble(&disasm);

	append(QString().sprintf("DBG_TRACE %x %s",
					msg.exception.except_record.ExceptionAddress,
					ud_insn_asm(&disasm))
				);
}

void log_window::breakpoint_slot(dbg_msg msg)
{
	append(QString().sprintf("DBG_BREAKPOINT %x",
					msg.exception.except_record.ExceptionAddress)
				);
}

void log_window::terminated_slot(dbg_msg msg)
{
	append(QString().sprintf("DBG_TERMINATED %x by %x",
					msg.terminated.proc_id,
					msg.process_id)
				);
}

void log_window::start_thread_slot(dbg_msg msg)
{
	append(QString().sprintf("DBG_START_THREAD %x by %x, teb: %x",
					msg.thread_start.thread_id,
					msg.process_id,
					msg.thread_start.teb_addr)
				);
}

void log_window::exit_thread_slot(dbg_msg msg)
{
	append(QString().sprintf("DBG_EXIT_THREAD %x in %x by %x",
					msg.thread_exit.thread_id,
					msg.thread_exit.proc_id,
					msg.process_id)
				);
}

void log_window::exception_slot(dbg_msg msg)
{
	append(QString().sprintf("DBG_EXCEPTION %0.8x in ADDR %x %x:%x",
					msg.exception.except_record.ExceptionCode,
					msg.exception.except_record.ExceptionAddress,
					msg.thread_id,
					msg.process_id)
				);
}

void log_window::dll_load_slot(dbg_msg msg)
{
	append(QString().sprintf("DBG_LOAD_DLL %s adr 0x%p sz 0x%x in %x:%x",
			QString::fromWCharArray(msg.dll_load.dll_name).toStdString().c_str(),
			msg.dll_load.dll_image_base,
			msg.dll_load.dll_image_size,
			msg.thread_id,
			msg.process_id)
		);
}