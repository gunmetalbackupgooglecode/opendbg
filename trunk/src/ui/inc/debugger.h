#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "disasm.h"
#include "utils.h"
#include "ngtracerapi.h"

class Debugger
{
public:
	Debugger();

	int init();

	void open_session(ULONG options, LPVOID params);
	void close_session();

	void load(const wxString &path, ULONG options);
	void unload();

	void set_callback(ULONG type, PVOID callback, PVOID pthis)
	{
		trc_set_callback(m_sesId, type, callback, pthis);
	}

	void set_tracemode(ULONG mode)
	{
		m_trace_mode = mode;
	}

	ULONG get_tracemode()
	{
		return m_trace_mode;
	}

	//static void __stdcall thunk_excpt_event(PTRC_EXCEPTION_EVENT evt, void *arg)
	//{
	//	static_cast<Debugger*>(arg)->excpt_event(evt);
	//}

	//static void __stdcall thunk_proc_event(PTRC_PROCESS_EVENT evt, void *arg)
	//{
	//	static_cast<Debugger*>(arg)->proc_event(evt);
	//}

	ULONG __stdcall excpt_event(PTRC_EXCEPTION_EVENT evt);
	ULONG __stdcall dbg_event(PTRC_EXCEPTION_EVENT evt);
	ULONG __stdcall proc_event(PTRC_PROCESS_EVENT evt);

public:
	ULONG m_sesId;

public:
	wxCriticalSection m_critsec;
	ULONG m_hitcount;
	
	ULONG m_trace_mode;

public:
	void bp_list();
	void threads_list();
	void mod_list();
	ULONG set_hwbp(char * bp);
	ULONG set_bp(char * bp);
	ULONG del_bp(char * bp);
	int load_target(char *target, ULONG options);
	void print_mem(char *mem_addr);
	void print_regs(ULONG tid);
	ULONG split_cmd(char * str,char **cmd,char **arg);
	ULONG parse_cmd(char* str);
};

#endif // __DEBUGGER_H__