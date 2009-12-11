/*! \file column.h
* \brief This class used as interface to debug api

This class will be reconstructed
* \author d1mk4
*/

#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "disasm.h"
#include "utils.h"
//#include "ngtracerapi.h"

class Debugger
{
public:
	Debugger();

	int init();

	void open_session(uint32_t options, LPVOID params);
	void close_session();

	void load(const wxString &path, uint32_t options);
	void unload();

	void set_callback(uint32_t type, PVOID callback, PVOID pthis)
	{
		trc_set_callback(m_sesId, type, callback, pthis);
	}

	void set_tracemode(uint32_t mode)
	{
		m_trace_mode = mode;
	}

	uint32_t get_tracemode()
	{
		return m_trace_mode;
	}

	uint32_t __stdcall excpt_event(PTRC_EXCEPTION_EVENT evt);
	uint32_t __stdcall dbg_event(PTRC_EXCEPTION_EVENT evt);
	uint32_t __stdcall proc_event(PTRC_PROCESS_EVENT evt);

public:
	uint32_t m_sesId;

public:
	wxCriticalSection m_critsec;
	uint32_t m_hitcount;
	
	uint32_t m_trace_mode;

public:
	void bp_list();
	void threads_list();
	void mod_list();
	uint32_t set_hwbp(char * bp);
	uint32_t set_bp(char * bp);
	uint32_t del_bp(char * bp);
	int load_target(char *target, uint32_t options);
	void print_mem(char *mem_addr);
	void print_regs(uint32_t tid);
	uint32_t split_cmd(char * str,char **cmd,char **arg);
	uint32_t parse_cmd(char* str);
};

#endif // __DEBUGGER_H__