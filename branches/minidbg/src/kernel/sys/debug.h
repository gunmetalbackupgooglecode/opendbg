#ifndef _DEBUG_
#define _DEBUG_

#include "syscall.h"

int init_debug(HANDLE h_key);

int dbg_open_process(syscall *data);
int dbg_terminate_process(syscall *data);
int dbg_attach_call(syscall *data);
int dbg_get_message(syscall *data);
int dbg_set_filter(syscall *data);
int dbg_continue(syscall *data);

#endif
