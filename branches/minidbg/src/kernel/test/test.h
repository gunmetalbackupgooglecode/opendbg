#ifndef _TEST_
#define _TEST_

#include <windows.h>
#include <cstdio>

#include "pdbparser.h"
#include "dbgapi.h"

namespace trc
{

static uintptr_t
CALLBACK get_symbols_callback(
	int sym_type,
	char * sym_name,
	char * sym_subname,
	pdb::pdb_parser& pdb
	)
{
	if (sym_type == SYM_TIMESTAMP)
	{
		return 0x45E53F9C;
	}

	if (sym_type == SYM_NTAPI_NUM)
	{
		if (strcmp(sym_name, "ZwTerminateProcess") == 0) {
			return 0x101;
		}

		if (strcmp(sym_name, "ZwCreateThread") == 0) {
			return 0x035;
		}

		if (strcmp(sym_name, "ZwTerminateThread") == 0) {
			return 0x102;
		}
	}

	wchar_t sym_name_w[255];
	MultiByteToWideChar(
		CP_ACP, 0, sym_name, strlen(sym_name)+1,
		sym_name_w, sizeof(sym_name_w)/sizeof(sym_name_w[0])
	);

	wchar_t sym_subname_w[255];
	if (sym_subname && strlen(sym_subname)) {
		MultiByteToWideChar(
			CP_ACP, 0, sym_subname, strlen(sym_subname)+1,
			sym_subname_w, sizeof(sym_subname_w)/sizeof(sym_subname_w[0])
		);
	}

	if (sym_type == SYM_OFFSET) {
		return pdb.get_symbol(sym_name_w).get_rva();
	}

	if (sym_type == SYM_STRUCT_OFFSET)
		return pdb.get_type(sym_name_w).get_member(sym_subname_w).get_offset();

	return 0;
}

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

private:
	int init()
	{
		return dbg_initialize_api(0x75ECB86B, L"C:\\ntoskrnl.pdb", (dbg_sym_get)get_symbols_callback);
	}
};

}

#endif
