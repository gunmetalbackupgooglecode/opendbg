#include "stdafx.h"
#include <io.h>
#include "DebuggerCLI.h"

namespace po = boost::program_options;
using namespace std;

DebuggerCLI::DebuggerCLI(void)
{
#ifdef _MSC_VER
	interactive = _isatty(_fileno(stdin)) != 0;
#else
	interactive = isatty(fileno(stdin)) != 0;
#endif
	desc.add_options()
		("show", po::value<string>()->implicit_value("info")->notifier(&ShowHandler), "show informational messages")
		("help,?", po::value<string>()->implicit_value("")->notifier(&HelpHandler), "show this message")
		("load,l", po::value<string>()->notifier(&LoadHandler), "load script file(s)")
		("start,s", po::value<string>()->notifier(&StartHandler), "start process(es)")
		("quit", po::value<int>()->implicit_value(0)->notifier(&ExitHandler), "close opendbg")
		;

	cli = new boost::cli::command_line_interpreter(desc, interactive ? "> " : "");
}

boost::cli::commands_description DebuggerCLI::desc = boost::cli::commands_description();

void DebuggerCLI::LoadHandler(string what)
{
	// TODO: script loading
	cout << "script loading is not supported" << endl;
}

void DebuggerCLI::StartHandler(string what)
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

void DebuggerCLI::Interpret(std::istream& input_stream)
{
	cli->interpret(input_stream);
}

void DebuggerCLI::HelpHandler(string param)
{
	// TODO: normal description
	//cout << desc;
}

void DebuggerCLI::ShowHandler(string what)
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

void DebuggerCLI::Start()
{
	cli->interpret(std::cin);
}

void DebuggerCLI::ExitHandler(int code)
{
	exit(code);
}

DebuggerCLI::~DebuggerCLI(void)
{
	delete cli;
}
