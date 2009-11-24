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
	~debugger(void);
	void debug_process(const std::string& imageName);
	u_long get_version();
	void test();

private:
	event_filt m_filter;
	HANDLE     m_pid;
	dbg_msg    *m_msg;

};

#endif
