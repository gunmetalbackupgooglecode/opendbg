#ifndef _DEBUGGERCLI_H_
#define _DEBUGGERCLI_H_

#include <string>

#include <boost/thread.hpp>
#include <boost/ref.hpp>

#include "command_line_interpreter.hpp"
#include "debugger.h"

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
	void show_handler(const std::string& param);
	void help_handler(const std::string& param);

	void load_handler(const std::string& param);
	
	void start_handler(const std::string& param);
	void trace_handler(const std::string& param);

	void exit_handler(int code);

private:
	boost::cli::command_line_interpreter* m_cli;

	debugger m_debugger;

	bool m_interactive;
	static boost::cli::commands_description m_desc;
};

#endif
