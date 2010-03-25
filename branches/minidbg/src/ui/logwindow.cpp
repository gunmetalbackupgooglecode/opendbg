#include "stdafx.h"
#include "logwindow.h"

#include "dbg_def.h"
#include "dbgapi.h"
#include "udis86.h"

const size_t max_instruction_len = 16;

void log_window::created_handler(dbg_msg msg)
{
	append(tr("started debugger\n"));
}

void log_window::debug_handler(dbg_msg msg)
{
	ud_t disasm;
	ud_init(&disasm);
	ud_set_mode(&disasm, 32);
	u8 buf[max_instruction_len];
	dbg_read_memory(msg.process_id, msg.exception.except_record.ExceptionAddress, &buf, max_instruction_len*sizeof(u8), NULL);
	ud_set_syntax(&disasm, UD_SYN_INTEL);
	ud_set_input_buffer(&disasm, buf, max_instruction_len);
	ud_disassemble(&disasm);

	QTextStream stream;
	stream << "\n" << msg.exception.except_record.ExceptionAddress << " " << ud_insn_asm(&disasm) << "\n";
	append(stream.readAll());
}

void log_window::breakpoint_handler(dbg_msg msg)
{
	QTextStream stream;
	stream << "breakpoint address: " << msg.exception.except_record.ExceptionAddress << "\n";
	append(stream.readAll());
}

//void log_window::terminated_handler(dbg_msg msg)
//{
//
//}
//
//void log_window::start_thread_handler(dbg_msg msg)
//{
//
//}
//
//void log_window::exit_thread_handler(dbg_msg msg)
//{
//
//}
//
//void log_window::exception_handler(dbg_msg msg)
//{
//
//}
//
//void log_window::dll_load_handler(dbg_msg msg)
//{
//
//}