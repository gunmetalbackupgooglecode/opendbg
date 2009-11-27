#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <string>
#include "dbgapi.h"
#include "pdbparser.h"

class debugger
{
private:
	static uintptr_t CALLBACK get_symbols_callback( int sym_type, char * sym_name, char * sym_subname, pdb::pdb_parser& pdb );

public:
	debugger();
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

private:
	std::string m_image_name;
	event_filt m_filter;
	HANDLE     m_pid;
	dbg_msg    *m_msg;
};

#endif
