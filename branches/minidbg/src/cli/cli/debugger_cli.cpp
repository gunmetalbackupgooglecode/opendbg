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
{
#ifdef _MSC_VER
	m_interactive = _isatty(_fileno(stdin)) != 0;
#else
	m_interactive = isatty(fileno(stdin)) != 0;
#endif

	m_desc.add_options()
		("show", po::value<std::string>()->implicit_value("info")->notifier(&show_handler), "show informational messages")
		("help,?", po::value<std::string>()->implicit_value("")->notifier(&help_handler), "show this message")
		("load,l", po::value<std::string>()->notifier(&load_handler), "load script file(s)")
		("start,s", po::value<std::string>()->notifier(&start_handler), "start process(es)")
		("quit", po::value<int>()->implicit_value(0)->notifier(&exit_handler), "close opendbg")
		;

	m_cli = new boost::cli::command_line_interpreter(m_desc, m_interactive ? "> " : "");
}

debugger_cli::~debugger_cli(void)
{
	delete m_cli;
}

void debugger_cli::load_handler(const std::string& filename)
{
	try {
	m_debugger.debug_process(filename);
	}
	
	catch (std::exception& e) {
		std::cout << e.what() << "\n";
	}
	
	std::cout << "script loading is not supported\n";
}

void debugger_cli::start_handler(const std::string& what)
{
	int pid;
	if (pid = atoi(what.data()))
	{
		HANDLE handle;
		DWORD nameLength = 100;
		std::auto_ptr<char> imageName(new char[nameLength]);
		if ((handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid)) == NULL)
		{
			std::cout << "no such process with pid " << what << "\n";
			return;
		}
		std::cout << "found existing process ";
		//if (QueryFullProcessImageNameA(handle, 0, imageName.get(), &nameLength))
		//	std::cout << imageName.get() << "\n";
		//else
		//	std::cout << "(can't get image name)\n";
	}

}

void debugger_cli::interpret(std::istream& input_stream)
{
	m_cli->interpret(input_stream);
}

void debugger_cli::help_handler(const std::string& param)
{
	// TODO: normal description
	//cout << desc;
}

void debugger_cli::show_handler(const std::string& what)
{
	if (what == "w")
	{
		std::cout << "This program is distributed in the hope that it will be useful,\n";
		std::cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of\n";
		std::cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n";
		std::cout << "GNU General Public License for more details.\n";
	}
	else if (what == "c")
	{
		std::cout << "This program is free software: you can redistribute it and/or modify\n";
		std::cout << "it under the terms of the GNU General Public License as published by\n";
		std::cout << "the Free Software Foundation, either version 3 of the License, or\n";
		std::cout << "(at your option) any later version.\n";
	}
	else if (what == "info")
	{
		std::cout << "please supply one of arguments:\n";
		std::cout << "\tinfo\tshow this message\n";
		std::cout << "\tw\tshow gpl warranty message\n";
		std::cout << "\tc\tshow gpl free software message\n";
	}
}

void debugger_cli::exit_handler(int code)
{
	exit(code);
}

void debugger_cli::start()
{
	m_cli->interpret(std::cin);
}
