#ifndef _DBG_DEF_
#define _DBG_DEF_

#include "defines.h"

#pragma pack (push, 1)

typedef struct _dbg_terminated {
	HANDLE proc_id;
	u32    exit_code;

} dbg_terminated;

typedef struct _dbg_start_thread {
	HANDLE   thread_id;
	void    *teb_addr;
	CONTEXT  initial_context;

} dbg_start_thread;

typedef struct _dbg_exit_thread {
	HANDLE thread_id;
	HANDLE proc_id;
	u32    exit_code;

} dbg_exit_thread;

typedef struct _dbg_exception {
	EXCEPTION_RECORD except_record;
	BOOLEAN          first_chance;

} dbg_exception;

typedef struct _dbg_dll_load {
	PVOID       dll_image_base;
	SIZE_T      dll_image_size;
	WCHAR		dll_name[MAX_PATH];
} dbg_dll_load;

typedef struct _dbg_msg {
	HANDLE process_id;
	HANDLE thread_id;
	u32    event_code;

	union
	{
		dbg_terminated   terminated;
		dbg_start_thread thread_start;
		dbg_exit_thread  thread_exit;
		dbg_exception    exception;
		dbg_dll_load	 dll_load;
	};

} dbg_msg;

typedef struct _cont_dat {
	HANDLE           proc_id;
	u32              status;
	EXCEPTION_RECORD new_record;

} cont_dat;

#define FILTER_MAX  100

typedef struct _except_filt {
	u32 filtr_from;
	u32 filtr_to;

} except_filt;

typedef struct _event_filt {
	u32         event_mask;
	u32         filtr_count;
	except_filt filters[FILTER_MAX];

} event_filt;

typedef struct _set_filter_data {
	HANDLE     process;
	event_filt filter;

} set_filter_data;

#pragma pack (pop)

#define DBG_EXCEPTION     1
#define DBG_TERMINATED    2
#define DBG_START_THREAD  4
#define DBG_EXIT_THREAD   8
#define DBG_LOAD_DLL      16
#define DBG_UNLOAD_DLL    32
#define DBG_CONTINUE_CALL 64
#define DBG_RDTSC         128

#define RES_NOT_HANDLED   0
#define RES_CONTINUE      1
#define RES_CORRECT_FRAME 2


#endif