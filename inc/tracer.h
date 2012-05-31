#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <boost/signals2.hpp>
#include <vector>

#include "dbg_def.h"
#include "dbgconst.h"

#include "dbgapi.h"
#include "pdbparser.h"
#include "../src/tracer/BreakpointsManager.h"

#include "udis86.h"

namespace debugger{

#define MAX_INSTRUCTION_LEN 16

enum instruction_set { X86 = 32, X86_64 = 64 };

class tracer
{
public:
	typedef boost::signals2::signal<void (dbg_msg&)> signal_t;
	typedef boost::signals2::connection             connection_t;
	//typedef std::vector<breakpoint>                 breakpoint_array_t;
	//typedef breakpoint_array_t::iterator            breakpoint_iterator;

public:
	tracer();
	tracer(instruction_set set);
	tracer(instruction_set set, const std::string& image_name);
	tracer(const tracer& dbg);

	void init(instruction_set set);
	void trace_process();
	void stop_process();
	u_long get_version();
	void open_process(const std::string& filename);
	// interfaces by Rascal
	BreakpointsManager& getBreakpointsManager();
	//void add_breakpoint(u32 proc_id, u32 thread_id, u3264 address);
	//void del_breakpoint(u32 proc_id, u32 thread_id, u3264 address);

	bool is_untraceable_opcode(u8* opcode);
	bool is_call();
	bool is_rep();
	bool is_loop();

	void step_into();
	void step_over();
	void step_out();

	void operator()() // this function is necessary for boost::thread
	{
		trace_process();
	}

	void set_image_name(const std::string& imagename)
	{
		m_image_name = imagename;
	}

	std::string get_image_name()
	{
		return m_image_name;
	}

	HANDLE get_pid() const
	{
		return m_pid;
	}

	connection_t add_created_slot(const signal_t::slot_type& subscriber)
	{
		return m_created_signal.connect(subscriber);
	}

	connection_t add_trace_slot(const signal_t::slot_type& subscriber)
	{
		return m_trace_signal.connect(subscriber);
	}

	connection_t add_breakpoint_slot(const signal_t::slot_type& subscriber)
	{
		return m_breakpoint_signal.connect(subscriber);
	}

	connection_t add_terminated_slot(const signal_t::slot_type& subscriber)
	{
		return m_terminated_signal.connect(subscriber);
	}

	connection_t add_start_thread_slot(const signal_t::slot_type& subscriber)
	{
		return m_start_thread_signal.connect(subscriber);
	}

	connection_t add_exit_thread_slot(const signal_t::slot_type& subscriber)
	{
		return m_exit_thread_signal.connect(subscriber);
	}

	connection_t add_exception_slot(const signal_t::slot_type& subscriber)
	{
		return m_exception_signal.connect(subscriber);
	}

	connection_t add_dll_load_slot(const signal_t::slot_type& subscriber)
	{
		return m_dll_load_signal.connect(subscriber);
	}

private:
	static uintptr_t CALLBACK get_symbols_callback(int sym_type, char * sym_name, char * sym_subname, pdb::pdb_parser& pdb);

private:
	std::string m_image_name;
	event_filt  m_filter;
	HANDLE      m_pid;
	HANDLE      m_tid;
	dbg_msg     m_msg;
	// interfaces by Rascal
	sys::PROCESS_PTR process;
	BreakpointsManager breakpointsManager;

	signal_t m_created_signal;
	signal_t m_trace_signal;
	signal_t m_breakpoint_signal;
	signal_t m_terminated_signal;
	signal_t m_start_thread_signal;
	signal_t m_exit_thread_signal;
	signal_t m_exception_signal;
	signal_t m_dll_load_signal;

	//breakpoint_array_t m_bp_usr_array;
	//breakpoint_array_t m_bp_trc_array;
	ud_t m_disasm;
};

bool enable_single_step(HANDLE process_id, HANDLE thread_id);
bool disable_single_step(HANDLE thread_id);

u32 get_size_till_ret(void* fn);

}

#endif
