#include "stdafx.h"
// isatty
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif
#include "debugger_cli.h"

namespace po = boost::program_options;

boost::cli::commands_description debugger_cli::m_desc = boost::cli::commands_description();

debugger_cli::debugger_cli()
 : m_tracer()
{
#ifdef _MSC_VER
	m_interactive = _isatty(_fileno(stdin)) != 0;
#else
	m_interactive = isatty(fileno(stdin)) != 0;
#endif

	m_desc.add_options()
		(
			"help,?",
			po::value<std::string>()->implicit_value("")->notifier(boost::bind(&debugger_cli::help_handler, this, _1)),
			"show this message"
		)

		(
			"load,l",
			po::value<std::string>()->notifier(boost::bind(&debugger_cli::load_handler, this, _1)),
			"load script file(s)"
		)

		(
			"open",
			po::value<std::string>()->notifier(boost::bind(&debugger_cli::open_handler, this, _1)),
			"start debugging process(es)"
		)

		(
			"step,s",
			po::value<std::string>()->implicit_value("0")->notifier(boost::bind(&debugger_cli::step_handler, this, _1)),
			"Step to next line of code. Will step into a function"
		)

		(
			"next,n",
			po::value<std::string>()->implicit_value("0")->notifier(boost::bind(&debugger_cli::next_handler, this, _1)),
			"Execute next line of code. Will not enter functions"
		)

		(
			"quit",
			po::value<int>()->implicit_value(0)->notifier(boost::bind(&debugger_cli::exit_handler, this, _1)),
			"close opendbg"
		)
		;

	m_tracer.add_created_slot(boost::bind(&debugger_cli::created_slot, this, _1));
	m_tracer.add_trace_slot(boost::bind(&debugger_cli::debug_slot, this, _1));
	m_tracer.add_breakpoint_slot(boost::bind(&debugger_cli::breakpoint_slot, this, _1));
	m_tracer.add_terminated_slot(boost::bind(&debugger_cli::terminated_slot, this, _1));
	m_tracer.add_start_thread_slot(boost::bind(&debugger_cli::start_thread_slot, this, _1));
	m_tracer.add_exit_thread_slot(boost::bind(&debugger_cli::exit_thread_slot, this, _1));
	m_tracer.add_exception_slot(boost::bind(&debugger_cli::exception_slot, this, _1));
	m_tracer.add_dll_load_slot(boost::bind(&debugger_cli::dll_load_slot, this, _1));

	m_cli = new boost::cli::command_line_interpreter(m_desc, m_interactive ? "> " : "");
}

debugger_cli::~debugger_cli(void)
{
	delete m_cli;
}

void debugger_cli::open_handler(const std::string& filename)
{
	m_tracer.set_image_name(filename);
	boost::thread dbg_thread(boost::ref(m_tracer));
}

void debugger_cli::load_handler(const std::string& filename)
{
	std::cout << "script loading is not supported\n";
}

void debugger_cli::run_handler(const std::string& param)
{
	m_condition.notify_one();
}

void debugger_cli::step_handler(const std::string& param)
{
	m_tracer.step_into();
	m_condition.notify_one();
}

void debugger_cli::help_handler(const std::string& param)
{
	// TODO: normal description
	//cout << desc;
}

void debugger_cli::next_handler(const std::string& param)
{
	m_tracer.step_over();
	m_condition.notify_one();
}

void debugger_cli::exit_handler(int code)
{
	exit(code);
}

void debugger_cli::show_handler(const std::string& param)
{
	if (param == "w")
	{
		std::cout << "This program is distributed in the hope that it will be useful,\n";
		std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
		std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
		std::cout << "GNU General Public License for more details.\n";
	}
	else if (param == "c")
	{
		std::cout << "This program is free software: you can redistribute it and/or modify\n";
		std::cout << "it under the terms of the GNU General Public License as published by\n";
		std::cout << "the Free Software Foundation, either version 3 of the License, or\n";
		std::cout << "(at your option) any later version.\n";
	}
	else if (param == "info")
	{
		std::cout << "please supply one of arguments:\n";
		std::cout << "\tinfo\tshow this message\n";
		std::cout << "\tw\tshow gpl warranty message\n";
		std::cout << "\tc\tshow gpl free software message\n";
	}
}

void debugger_cli::interpret(std::istream& input_stream)
{
	m_cli->interpret(input_stream);
}

void debugger_cli::start()
{
	m_cli->interpret(std::cin);
}

void debugger_cli::debug_slot(dbg_msg msg)
{
	ud_t disasm;
	ud_init(&disasm);
	ud_set_mode(&disasm, 32);
	u8 buf[MAX_INSTRUCTION_LEN];
	dbg_read_memory(m_tracer.get_pid(), msg.exception.except_record.ExceptionAddress, &buf, MAX_INSTRUCTION_LEN*sizeof(u8), NULL);
	ud_set_syntax(&disasm, UD_SYN_INTEL);
	ud_set_input_buffer(&disasm, buf, MAX_INSTRUCTION_LEN);
	ud_disassemble(&disasm);

	std::cout << "\n" << msg.exception.except_record.ExceptionAddress << " " << ud_insn_asm(&disasm) << "\n";
	lock_t lock(m_mutex);
	m_condition.wait(lock);
	//std::cout << msg.event_code << "\n";
}

void debugger_cli::created_slot( dbg_msg msg )
{
	std::cout << "started debugger\n";
}

void debugger_cli::breakpoint_slot( dbg_msg msg )
{
	std::cout << "breakpoint address: " << msg.exception.except_record.ExceptionAddress << "\n";
	lock_t lock(m_mutex);
	m_condition.wait(lock);
}

void debugger_cli::terminated_slot( dbg_msg msg )
{
	printf("DBG_TERMINATED %x by %x\n",
					msg.terminated.proc_id,
					msg.process_id
					);
}

void debugger_cli::start_thread_slot( dbg_msg msg )
{
	printf("DBG_START_THREAD %x by %x, teb: %x\n",
					msg.thread_start.thread_id,
					msg.process_id,
					msg.thread_start.teb_addr
					);
}

void debugger_cli::exit_thread_slot( dbg_msg msg )
{
	printf("DBG_EXIT_THREAD %x in %x by %x\n",
					msg.thread_exit.thread_id,
					msg.thread_exit.proc_id,
					msg.process_id
					);
}

void debugger_cli::exception_slot( dbg_msg msg )
{
	printf("DBG_EXCEPTION %0.8x in ADDR %x %x:%x\n",
					msg.exception.except_record.ExceptionCode,
					msg.exception.except_record.ExceptionAddress,
					msg.thread_id,
					msg.process_id
					);

	lock_t lock(m_mutex);
	m_condition.wait(lock);
}

void debugger_cli::dll_load_slot( dbg_msg msg )
{
	printf("DBG_LOAD_DLL %ws adr 0x%p sz 0x%x in %x:%x\n",
		msg.dll_load.dll_name,
		msg.dll_load.dll_image_base,
		msg.dll_load.dll_image_size,
		msg.thread_id,
		msg.process_id
		);
}

