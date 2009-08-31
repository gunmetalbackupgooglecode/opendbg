#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <string>
#include "pdbparser.h"

class Debugger
{
	event_filt filter;
	HANDLE     pid;
	dbg_msg    *msg;
	static uintptr_t CALLBACK get_symbols_callback( int sym_type, char * sym_name, char * sym_subname, pdb::pdb_parser& pdb );
public:
	Debugger();
	~Debugger(void);
	void DebugProcess(std::string imageName);
	u_long GetVersion();
	void Test();
};

#endif
