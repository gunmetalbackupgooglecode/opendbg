#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <string>
#include <vector>

#include <boost/function.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>

#include "dbgapi.h"
#include "pdbparser.h"
#include "breakpoint.h"

namespace trc
{

class tracer
{
public:
	typedef boost::signals2::signal<void (dbg_msg)> signal_t;
	typedef boost::signals2::connection      connection_t;
	typedef std::vector<breakpoint>          breakpoint_array_t;
	typedef breakpoint_array_t::iterator     breakpoint_iterator;

public:
	tracer();
	tracer(const tracer& dbg)
	 : m_image_name(dbg.m_image_name),
	   m_pid(dbg.m_pid)
	{
		m_filter.event_mask = dbg.m_filter.event_mask;
		m_filter.filtr_count = dbg.m_filter.filtr_count;
		for (size_t i = 0; i < dbg.m_filter.filtr_count; ++i)
			m_filter.filters[i] = dbg.m_filter.filters[i];
	}

	tracer(const std::string& image_name);
	void init();
	void trace_process();
	u_long get_version();
	void open_process(const std::string& filename);
	void add_breakpoint(u32 proc_id, u32 thread_id, u3264 address);
	void del_breakpoint(u32 proc_id, u32 thread_id, u3264 address);

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

	void operator()() // this function is necessary for boost::thread
	{
		trace_process();
	}

private:
	static uintptr_t CALLBACK get_symbols_callback(int sym_type, char * sym_name, char * sym_subname, pdb::pdb_parser& pdb);

private:
	std::string m_image_name;
	event_filt  m_filter;
	HANDLE      m_pid;
	dbg_msg     m_msg;

	signal_t m_trace_signal;
	signal_t m_breakpoint_signal;
	signal_t m_terminated_signal;
	signal_t m_start_thread_signal;
	signal_t m_exit_thread_signal;
	signal_t m_exception_signal;
	signal_t m_dll_load_signal;

	breakpoint_array_t m_bp_array;

};

bool enable_single_step(HANDLE process_id, HANDLE thread_id);
bool disable_single_step(HANDLE thread_id);

}

#endif
