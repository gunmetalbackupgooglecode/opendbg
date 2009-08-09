/*
    *
    * Copyright (c) 2008
    * ntldr <ntldr@freed0m.org> PGP key ID - 0xC48251EB4F8E4E6E
    *

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <intrin.h>
#include "dbgapi.h"
#include "ntdll.h"
#include "..\sys\syscall.h"

static struct {
    int   sym_type;
    char *sym_name;
    char *sym_subname;
} req_symbols[] =
{
    //{ SYM_TIMESTAMP,     "timestamp",               NULL },
    { SYM_NTAPI_NUM,     "ZwTerminateProcess",      NULL },
    { SYM_NTAPI_NUM,     "ZwCreateThread",          NULL },
    { SYM_NTAPI_NUM,     "ZwTerminateThread",       NULL },
    { SYM_OFFSET,        "_NtTerminateProcess@8",   NULL },
    { SYM_OFFSET,        "_NtResumeThread@8",       NULL },
    { SYM_OFFSET,        "_NtCreateThread@32",      NULL },
    { SYM_OFFSET,        "_NtTerminateThread@8",    NULL },
    { SYM_OFFSET,        "_KiDispatchException@20", NULL },
    { SYM_STRUCT_OFFSET, "_ETHREAD",                "ThreadListEntry" }
};

#define NUM_SYM (sizeof(req_symbols)/sizeof(req_symbols[0]))

static const char driver_name[] = "dbgapi.sys";
static u32  acc_key;

static SC_HANDLE dbg_install_sc(const char *path, const char *name)
{
    SC_HANDLE h_scm;
    SC_HANDLE h_svc = NULL;

    if (h_scm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS))
    {
        h_svc = CreateServiceA(
                  h_scm, name,
                    name, SERVICE_ALL_ACCESS,
                    SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
                    SERVICE_ERROR_IGNORE, path,
                    NULL, NULL, NULL, NULL, NULL
                    );

        if (h_svc == NULL) {
            h_svc = OpenServiceA(h_scm, name, SERVICE_ALL_ACCESS);
        }

        CloseServiceHandle(h_scm);
    }

    return h_svc;
}

DBGAPI_API
HANDLE dbg_create_process(
        IN PVOID remote_id,
        IN PCHAR cmd_line,
        IN ULONG create_flags
        )
{
    STARTUPINFOA        str = {0};
    PROCESS_INFORMATION pri;
    BOOL                b_succ;

    str.cb = sizeof(str);

    b_succ = CreateProcessA(
               NULL, cmd_line,
                NULL, NULL,
                FALSE, create_flags,
                NULL, NULL,
                &str, &pri
                );

    if (b_succ == FALSE)  return 0;

    CloseHandle(pri.hProcess);
    CloseHandle(pri.hThread);

    return (HANDLE)pri.dwProcessId;
}


int dbg_build_start_params( cs8 *drv_name, u32 acc_key, const wchar_t* pdb_path, dbg_sym_get sym_callback )
{
    char  name[MAX_PATH];
    HKEY  h_key = NULL;
    int   i, succs = 0;
    char *s_name, *s_subname;
    int   s_type;
    u32   sym;
    pdb::pdb_parser pdb(pdb_path);

    /* build registry key path */
    _snprintf(
        name, sizeof(name),
        "SYSTEM\\CurrentControlSet\\Services\\%s", drv_name
        );

    // TODO: I don't understand - what is it?!
    // It's probably beginning of the pdb support
    // But for now this feature is not necessary
    // How turn off it?! I can't... ;(
    do
    {
        if (RegOpenKey(HKEY_LOCAL_MACHINE, name, &h_key) != 0) break;

        for (i = 0; i < NUM_SYM; i++)
        {
            s_type    = req_symbols[i].sym_type;
            s_name    = req_symbols[i].sym_name;
            s_subname = req_symbols[i].sym_subname;

            if ( (sym = sym_callback(s_type, s_name, s_subname, pdb)) == 0 ) break;

            if (s_type != SYM_STRUCT_OFFSET)
            {
                _snprintf(
                    name, sizeof(name),
                    "sym_%s", s_name
                    );
            }
            else
            {
                _snprintf(
                    name, sizeof(name),
                    "sym_%s.%s", s_name, s_subname
                    );
            }

            if (RegSetValueEx(h_key, name, 0, REG_DWORD, (BYTE*)pv(&sym), sizeof(sym)) != 0) break;
        }

        if (i != NUM_SYM) break;

        /* save access key */
        if (RegSetValueEx(h_key, "sc_key", 0, REG_DWORD, (BYTE*)pv(&acc_key), sizeof(u32)) != 0) break;
        succs = 1;
    } while (0);

    if (h_key != NULL) RegCloseKey(h_key);

    return succs;
}

static int dbg_syscall(
            int num, void *in_data,
            u32 in_size, void *out_data,
            u32 out_size
            )
{
    syscall call;

    call.number   = num;
    call.in_data  = in_data;
    call.in_size  = in_size;
    call.out_data = out_data;
    call.out_size = out_size;

    return (NtTerminateProcess(&call, acc_key) == STATUS_SUCCESS);
}

#define SC_GET_VERSION        0 /* get dbgapi driver version */
#define SC_OPEN_PROCESS       1
#define SC_TERMINATE_PROCESS  2
#define SC_DBG_ATTACH         3
#define SC_DBG_GET_MSG        4
#define SC_DBG_SET_FILTER     5
#define SC_DBG_COUNTINUE      6

DBGAPI_API u_long dbg_drv_version()
{
    u32 ver;

    if (dbg_syscall(SC_GET_VERSION, NULL, 0, &ver, sizeof(ver)) == 0)
    {
        ver = 0;
    }

    return ver;
}

static
HANDLE dbg_open_process(HANDLE pid)
{
    HANDLE h_proc;

    if (dbg_syscall(SC_OPEN_PROCESS, &pid, sizeof(pid), &h_proc, sizeof(h_proc)) == 0) {
        h_proc = NULL;
    }

    return h_proc;
}


DBGAPI_API
int dbg_terminate_process(
        IN PVOID  remote_id,
        IN HANDLE proc_id
        )
{
    HANDLE h_proc = NULL;
    int    succs  = 0;

    do
    {
        if ( (h_proc = dbg_open_process(proc_id)) == NULL ) {
            break;
        }

        if (dbg_syscall(SC_TERMINATE_PROCESS, &h_proc, sizeof(h_proc), NULL, 0) != 0) {
            succs = 1;
        }
    } while (0);

    if (h_proc != NULL) {
        CloseHandle(h_proc);
    }

    return succs;
}

DBGAPI_API
int dbg_attach_debugger(
        IN PVOID  remote_id,
        IN HANDLE proc_id
        )
{
    int succs = 0;

    if (dbg_syscall(SC_DBG_ATTACH, &proc_id, sizeof(proc_id), NULL, 0) != 0) {
        succs = 1;
    }

    return succs;
}

DBGAPI_API
int dbg_get_msg_event(
        PVOID    remote_id,
        HANDLE   proc_id,
        dbg_msg *msg
        )
{
    int succs = 0;

    if (dbg_syscall(SC_DBG_GET_MSG, &proc_id, sizeof(proc_id), msg, sizeof(dbg_msg)) != 0) {
        succs = 1;
    }

    return succs;
}

DBGAPI_API
int dbg_countinue_event(
        PVOID             remote_id,
        HANDLE            proc_id,
        u32               status,
        PEXCEPTION_RECORD new_record
        )
{
    cont_dat cont;
    int      succs = 0;

    cont.proc_id = proc_id;
    cont.status  = status;

    if (new_record != NULL)
    {
        fastcpy(
            &cont.new_record, new_record, sizeof(EXCEPTION_RECORD)
            );
    }

    if (dbg_syscall(SC_DBG_COUNTINUE, &cont, sizeof(cont), NULL, 0) != 0) {
        succs = 1;
    }

    return succs;
}

DBGAPI_API
int dbg_set_filter(
       PVOID       remote_id,
       HANDLE      proc_id,
       event_filt *filter
       )
{
    set_filter_data set_data;
    int             succs = 0;

    set_data.process = proc_id;

    memcpy(
        &set_data.filter, filter, sizeof(event_filt)
        );

    if (dbg_syscall(SC_DBG_SET_FILTER, &set_data, sizeof(set_data), NULL, 0) != 0) {
        succs = 1;
    }

    return succs;
}

/* some Win32 API stubs */
DBGAPI_API
int dbg_read_memory(
       PVOID       remote_id,
       HANDLE      proc_id,
       PVOID       mem_addr,
       PVOID       loc_buff,
       u_long      mem_size,
       u_long     *readed
       )
{
    HANDLE h_proc = NULL;
    int    succs  = 0;

    do
    {
        if ( (h_proc = dbg_open_process(proc_id)) == NULL ) {
            break;
        }

        if (ReadProcessMemory(h_proc, mem_addr, loc_buff, mem_size, readed)) {
            succs = 1;
        }
    } while (0);

    if (h_proc != NULL) {
        CloseHandle(h_proc);
    }

    return succs;
}

DBGAPI_API
int dbg_write_memory(
       PVOID       remote_id,
       HANDLE      proc_id,
       PVOID       mem_addr,
       PVOID       loc_buff,
       u_long      mem_size,
       u_long     *written
       )
{
    HANDLE h_proc = NULL;
    int    succs  = 0;

    do
    {
        if ( (h_proc = dbg_open_process(proc_id)) == NULL ) {
            break;
        }

        if (WriteProcessMemory(h_proc, mem_addr, loc_buff, mem_size, written)) {
            succs = 1;
        }
    } while (0);

    if (h_proc != NULL) {
        CloseHandle(h_proc);
    }

    return succs;
}

DBGAPI_API
int dbg_get_context(
       PVOID       remote_id,
       HANDLE      thread_id,
       PCONTEXT    context
       )
{
    HANDLE h_thread = NULL;
    int    succs    = 0;

    do
    {
        if ( (h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, (DWORD)thread_id)) == NULL )
        {
            break;
        }

        if (GetThreadContext(h_thread, context))
        {
            succs = 1;
        }
    } while (0);

    if (h_thread != NULL)
    {
        CloseHandle(h_thread);
    }

    return succs;
}

DBGAPI_API
int dbg_set_context(
       PVOID       remote_id,
       HANDLE      thread_id,
       PCONTEXT    context
       )
{
    HANDLE h_thread = NULL;
    int    succs    = 0;

    do
    {
        if ( (h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, (DWORD)thread_id)) == NULL ) {
            break;
        }

        if (SetThreadContext(h_thread, context)) {
            succs = 1;
        }
    } while (0);

    if (h_thread != NULL) {
        CloseHandle(h_thread);
    }

    return succs;
}

DBGAPI_API
int dbg_initialize_api(
       u_long      access_key,
       const wchar_t* pdb_path,
       dbg_sym_get sym_callback
       )
{
    char      drv_path[MAX_PATH] = {0};
    char     *p = &drv_path[MAX_PATH];
    int       succs = 0;
    SC_HANDLE h_svc;

    acc_key = access_key;

    if (dbg_drv_version() != 0) {
        return 1;
    }

    GetModuleFileNameA(
        NULL, drv_path, sizeof(drv_path)
        );

    while (*p != '\\') p--;

    strcpy(p+1, driver_name);

    if (h_svc = dbg_install_sc(drv_path, driver_name))
    {
        if (dbg_build_start_params(driver_name, acc_key, pdb_path, sym_callback) != 0)
        {
            succs = StartService(h_svc, 0, NULL);
        }

        DeleteService(h_svc);
        CloseServiceHandle(h_svc);
    }

    return succs;
}

static
void enable_debug_privilegies()
{
    HANDLE TTokenHd;
    TOKEN_PRIVILEGES TTokenPvg, rTTokenPvg;
    ULONG cbtpPrevious, pcbtpPreviousRequired;

    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TTokenHd))
    {
        LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &TTokenPvg.Privileges[0].Luid);
        TTokenPvg.PrivilegeCount = 1;
        TTokenPvg.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        cbtpPrevious = sizeof(rTTokenPvg);
        pcbtpPreviousRequired = 0;
        AdjustTokenPrivileges(TTokenHd, FALSE, &TTokenPvg, cbtpPrevious, &rTTokenPvg, &pcbtpPreviousRequired);
    }
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                    )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            enable_debug_privilegies();
        break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

/*!
    Stupid thunk
*/
DBGAPI_API
HFILE dbg_open_file(
        IN  LPCSTR lpFileName,
        OUT LPOFSTRUCT lpReOpenBuff,
        IN  UINT uStyle
        )
{
    return OpenFile(lpFileName, lpReOpenBuff, uStyle);
}

/*!
    Stupid thunk
*/
DBGAPI_API
BOOL dbg_close_file(
        IN  HANDLE hObject
        )
{
    return CloseHandle(hObject);
}

/*!
    Stupid thunk
*/
DBGAPI_API
BOOL dbg_read_file(
        IN  HANDLE hFile,
        OUT LPVOID lpBuffer,
        IN  DWORD nNumberOfBytesToRead,
        OUT LPDWORD lpNumberOfBytesRead,
        IN  LPOVERLAPPED lpOverlapped
        )
{
    return ReadFile(hFile,
                    lpBuffer,
                    nNumberOfBytesToRead,
                    lpNumberOfBytesRead,
                    lpOverlapped);
}

/*!
    Stupid thunk
*/
DBGAPI_API
DWORD dbg_resume_thread(
        IN  HANDLE hThread
        )
{
    return ResumeThread(hThread);
}

/*!
    Stupid thunk
*/
DBGAPI_API
DWORD dbg_suspend_thread(
        IN HANDLE hThread
        )
{
    return SuspendThread(hThread);
}

/*!
    Stupid thunk
*/
DBGAPI_API
int dbg_set_rdtsc(
        IN PVOID   context,
        IN BOOLEAN rdtsc_on
        )
{
    return -1;
}

/*!
    Stupid thunk
*/
DBGAPI_API
void dbg_close_thread(
        IN PVOID dbg_thread
        )
{
}

/*!
    Stupid thunk
*/
DBGAPI_API
PVOID dbg_open_thread(
        IN PVOID context,
        IN ULONG thread_id
        )
{
    return INVALID_HANDLE_VALUE;
}

/*!
    Stupid thunk
*/
DBGAPI_API
PVOID dbg_enum_processes(
        IN PVOID remote_id
        )
{
    return INVALID_HANDLE_VALUE;
}


/*!
    Stupid thunk
*/
DBGAPI_API
PVOID dbg_enum_modules(
        IN PVOID context
        )
{
    return INVALID_HANDLE_VALUE;
}


/*!
    Stupid thunk
*/
DBGAPI_API
PVOID dbg_enum_threads(
        IN PVOID remote_id,
        IN ULONG process_id
        )
{
    return INVALID_HANDLE_VALUE;
}

/*!
    Stupid thunk
*/
DBGAPI_API
void dbg_free_memory(void *memory)
{
}

/*!
    Stupid thunk
*/
DBGAPI_API
int dbg_get_thread_ctx(
        IN  PVOID dbg_thread,
        OUT PVOID trd_context
        )
{
    return -1;
}

/*!
    Stupid thunk
*/
DBGAPI_API
void dbg_detach_debugger(
        IN PVOID context
        )
{
}

/*!
    Stupid thunk
*/
DBGAPI_API
int dbg_hook_page(
        IN PVOID context,
        IN PVOID page_addr,
        IN PVOID code_page
        )
{
    return -1;
}

