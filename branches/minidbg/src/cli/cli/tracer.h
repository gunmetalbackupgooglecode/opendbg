#ifndef TRACER_H__
#define TRACER_H__

#include <windows.h>
#include <cstdio>
#include <vector>
#include <algorithm>

#include <boost/bind/bind.hpp>

#include "breakpoint.h"
#include "tracer_error.h"
#include "pdbparser.h"
#include "dbgapi.h"

namespace trc
{

uintptr_t CALLBACK get_symbols_callback(
	int sym_type,
	char * sym_name,
	char * sym_subname,
	pdb::pdb_parser& pdb
	);

class tracer
{
typedef std::vector<breakpoint> bp_array_type;
typedef bp_array_type::iterator bp_iterator;

public:
	tracer()
	{
		if(!init())
			throw tracer_error("can't initialize tracer");
	}
	
	void open_process(const std::string& filename);
	bool enable_single_step(HANDLE process_id, HANDLE thread_id);
	bool disable_single_step(HANDLE thread_id);

	void add_breakpoint(u32 proc_id, u32 thread_id, u3264 address);
	void del_breakpoint(u32 proc_id, u32 thread_id, u3264 address);

	HANDLE get_pid()
	{
		return m_pid;
	}

private:
	int init()
	{
		return dbg_initialize_api(0x75ECB86B, L"C:\\ntoskrnl.pdb", (dbg_sym_get)get_symbols_callback);
	}

private:
	HANDLE m_pid;
	bp_array_type m_bp_array;
};

}

#endif // TRACER_H__