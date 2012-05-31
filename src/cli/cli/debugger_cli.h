#ifndef _DEBUGGERCLI_H_
#define _DEBUGGERCLI_H_

#include "command_line_interpreter.hpp"
#include "tracer.h"

class debugger_cli
{
public:
	typedef boost::mutex              mutex_t;
	typedef boost::mutex::scoped_lock lock_t;
	typedef boost::condition_variable condition_t;

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
	void run_handler(const std::string& param);
	void show_handler(const std::string& param);
	void help_handler(const std::string& param);
	void load_handler(const std::string& param);
	
	void step_handler(const std::string& param);
	//void stepover_handler(const std::string& param);
	//void stepout_handler(const std::string& param);

	void exit_handler(int code);
	void next_handler(const std::string& param);
	void open_handler(const std::string& param);

	void created_slot(dbg_msg msg);
	void debug_slot(dbg_msg msg);
	void breakpoint_slot(dbg_msg msg);
	void terminated_slot(dbg_msg msg);
	void start_thread_slot(dbg_msg msg);
	void exit_thread_slot(dbg_msg msg);
	void exception_slot(dbg_msg msg);
	void dll_load_slot(dbg_msg msg);

private:
	boost::cli::command_line_interpreter* m_cli;

	debugger::tracer m_tracer;
	condition_t m_condition;
	mutex_t     m_mutex;

	bool m_interactive;
	static boost::cli::commands_description m_desc;
};

#endif
