#ifndef TRACER_H__
#define TRACER_H__

#include <windows.h>
#include <cstdio>

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

class tracer_error : public std::exception
{
public:
	explicit tracer_error(const std::string& msg)
	 : m_msg(msg)
	{}

	virtual ~tracer_error() throw() // destroy the object
	{}

	virtual const char* what() const throw() // return pointer to message string
	{
		return m_msg.c_str();
	}

private:
	std::string m_msg;
};

class tracer
{
public:
	tracer()
	{
		if(!init())
			throw tracer_error("can't initialize tracer");
	}
	
	void open_process(const std::string& filename);
	bool enable_single_step(HANDLE thread_id);
	bool disable_single_step(HANDLE thread_id);

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
	struct INSTRUCTION instr;
	struct PARAMS params;
};

}

#endif // TRACER_H__