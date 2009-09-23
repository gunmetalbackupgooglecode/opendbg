#ifndef _DEBUGGERCLI_H_
#define _DEBUGGERCLI_H_

#include <string>
#include "command_line_interpreter.hpp"
#include "debugger.h"

using namespace std;

class debugger_cli
{
	boost::cli::command_line_interpreter* m_cli;
	static boost::cli::commands_description m_desc;
	debugger m_debugger;
	bool m_interactive;
	static void show_handler(string what);
	static void help_handler(string param);
	static void exit_handler(int code);
	static void load_handler(string what);
	static void start_handler(string what);
public:
	debugger_cli(void);
	~debugger_cli(void);
	const bool& is_interactive() const { return m_interactive; }
	void set_interactive(bool interactive) { m_interactive = interactive; m_cli->Prompt() = m_interactive? "> " : ""; }
	void start();
	void interpret(istream& input_stream);
};

#endif
