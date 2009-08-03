#ifndef _DRIVER_
#define _DRIVER_

#include "syscall.h"

int dbg_get_version(syscall *data);

#define DBG_VERSION 1 /* dbgapi driver version */


extern void *ntkrn_base;

#endif