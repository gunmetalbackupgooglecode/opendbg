#ifndef _DEBUGGERCLI_H_
#define _DEBUGGERCLI_H_

#include <string>
#include "command_line_interpreter.hpp"
#include "debugger.h"

class param_cli
{
public:
	param_cli()
	 : m_what()
	{
		m_debugger = 0;
	}

	param_cli(debugger* dbg, const std::string& what)
	 : m_what(what)
	{
		m_debugger = dbg;
	}


	friend std::ostream &operator << (std::ostream& stream, param_cli o)
	{
		stream << o.m_what << "\n";
		return stream;
	}

	friend std::istream &operator >> (std::istream& stream, param_cli& o)
	{
		stream >> o.m_what;
		return stream;
	}
	
private:
	debugger*   m_debugger;
	std::string m_what;
};

static debugger m_debugger;

class debugger_cli
{
public:
	debugger_cli();
	~debugger_cli();

	const bool& is_interactive() const
	{
		return m_interactive;
	}

	void set_interactive(bool interactive)
	{
		m_interactive = interactive;
		m_cli->Prompt() = m_interactive ? "> " : "";
	}

	void start();
	void interpret(std::istream& input_stream);

private:
	static void show_handler(const std::string& what);
	static void help_handler(const std::string& param);
	static void load_handler(const std::string& what);
	static void start_handler(const std::string& what);
	static void exit_handler(int code);

private:
	boost::cli::command_line_interpreter* m_cli;

	bool m_interactive;
	static boost::cli::commands_description m_desc;
};

#endif
