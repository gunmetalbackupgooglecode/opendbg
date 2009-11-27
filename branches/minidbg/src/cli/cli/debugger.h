#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <string>

#include <boost/signals2.hpp>

#include "dbgapi.h"
#include "pdbparser.h"

class debugger
{
public:
	typedef boost::signals2::signal<void (dbg_msg)> signal_t;
	typedef boost::signals2::connection      connection_t;

public:
	debugger();
	debugger(const debugger& dbg)
	 : m_image_name(dbg.m_image_name),
	   m_pid(dbg.m_pid)
	{
		m_msg = new dbg_msg();
		//::memcpy(&this->m_msg, &dbg.m_msg, sizeof(dbg_msg));

		m_filter.event_mask = dbg.m_filter.event_mask;
		m_filter.filtr_count = dbg.m_filter.filtr_count;
		for (size_t i = 0; i < dbg.m_filter.filtr_count; ++i)
			m_filter.filters[i] = dbg.m_filter.filters[i];
	}

	debugger(const std::string& image_name);
	~debugger();
	
	void init();
	
	void debug_process();
	u_long get_version();
	
	void operator()()
	{
		debug_process();
	}

	HANDLE get_pid() const
	{
		return m_pid;
	}

	void set_image_name(const std::string& imagename)
	{
		m_image_name = imagename;
	}

	connection_t add_trace_slot(const signal_t::slot_type& subscriber)
	{
		return m_trace_signal.connect(subscriber);
	}

	connection_t add_breakpoint_slot(const signal_t::slot_type& subscriber)
	{
		return m_breakpoint_signal.connect(subscriber);
	}

private:
	static uintptr_t CALLBACK get_symbols_callback(int sym_type, char * sym_name, char * sym_subname, pdb::pdb_parser& pdb);

private:
	std::string m_image_name;
	event_filt  m_filter;
	HANDLE      m_pid;
	dbg_msg     *m_msg;

	signal_t m_trace_signal;
	signal_t m_breakpoint_signal;
};

#endif
