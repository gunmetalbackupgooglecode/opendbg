#ifndef _DEBUGGERCLI_H_
#define _DEBUGGERCLI_H_

#include <string>
#include "command_line_interpreter.hpp"
#include "Debugger.h"

using namespace std;

class DebuggerCLI
{
	boost::cli::command_line_interpreter* cli;
	static boost::cli::commands_description desc;
	Debugger debugger;
	bool interactive;
	static void ShowHandler(string what);
	static void HelpHandler(string param);
	static void ExitHandler(int code);
	static void LoadHandler(string what);
	static void StartHandler(string what);
public:
	DebuggerCLI(void);
	~DebuggerCLI(void);
	const bool& IsInteractive() const { return interactive; }
	void SetInteractive(bool _interactive) { interactive = _interactive; cli->Prompt() = interactive? "> " : ""; }
	void Start();
	void Interpret(istream& input_stream);
};

#endif
