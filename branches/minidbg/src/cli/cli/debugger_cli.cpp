#include "stdafx.h"
#include <io.h>
#include "debugger_cli.h"

namespace po = boost::program_options;
using namespace std;

debugger_cli::debugger_cli(void)
{
#ifdef _MSC_VER
	m_interactive = _isatty(_fileno(stdin)) != 0;
#else
	m_interactive = isatty(fileno(stdin)) != 0;
#endif
	m_desc.add_options()
		("show", po::value<string>()->implicit_value("info")->notifier(&show_handler), "show informational messages")
		("help,?", po::value<string>()->implicit_value("")->notifier(&help_handler), "show this message")
		("load,l", po::value<string>()->notifier(&load_handler), "load script file(s)")
		("start,s", po::value<string>()->notifier(&start_handler), "start process(es)")
		("quit", po::value<int>()->implicit_value(0)->notifier(&exit_handler), "close opendbg")
		;

	m_cli = new boost::cli::command_line_interpreter(m_desc, m_interactive ? "> " : "");
}

boost::cli::commands_description debugger_cli::m_desc = boost::cli::commands_description();

void debugger_cli::load_handler(string what)
{
	// TODO: script loading
	cout << "script loading is not supported" << endl;
}

void debugger_cli::start_handler(string what)
{
	int pid;
	if (pid = atoi(what.data()))
	{
		HANDLE handle;
		DWORD nameLength = 100;
		auto_ptr<char> imageName(new char[nameLength]);
		if ((handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid)) == NULL)
		{
			cout << "no such process with pid " << what << endl;
			return;
		}
		cout << "found existing process ";
		if (QueryFullProcessImageNameA(handle, 0, imageName.get(), &nameLength))
		{
			cout << imageName.get() << endl;
		}
		else
			cout << "(can't get image name)" << endl;
	}

}

void debugger_cli::interpret(std::istream& input_stream)
{
	m_cli->interpret(input_stream);
}

void debugger_cli::help_handler(string param)
{
	// TODO: normal description
	//cout << desc;
}

void debugger_cli::show_handler(string what)
{
	if (what == "w")
	{
		cout << "This program is distributed in the hope that it will be useful," << endl;
		cout << "but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl;
		cout << "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" << endl;
		cout << "GNU General Public License for more details." << endl;
	}
	else if (what == "c")
	{
		cout << "This program is free software: you can redistribute it and/or modify" << endl;
		cout << "it under the terms of the GNU General Public License as published by" << endl;
		cout << "the Free Software Foundation, either version 3 of the License, or" << endl;
		cout << "(at your option) any later version." << endl;
	}
	else if (what == "info")
	{
		cout << "please supply one of arguments:" << endl;
		cout << "\tinfo\tshow this message" << endl;
		cout << "\tw\tshow gpl warranty message" << endl;
		cout << "\tc\tshow gpl free software message" << endl;
	}
}

void debugger_cli::start()
{
	m_cli->interpret(std::cin);
}

void debugger_cli::exit_handler(int code)
{
	exit(code);
}

debugger_cli::~debugger_cli(void)
{
	delete m_cli;
}
