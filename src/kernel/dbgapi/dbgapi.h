#define DBGAPI_API __declspec(dllexport) 

#include "..\sys\dbg_def.h"

#define SYM_TIMESTAMP     0 /* timestamp of kernel image */
#define SYM_NTAPI_NUM     1 /* number of Nt api          */
#define SYM_OFFSET        2 /* global symbol offset      */
#define SYM_STRUCT_OFFSET 3 /* structure field offset    */

typedef 
u_long (CALLBACK *dbg_sym_get)(
        int  sym_type,
        char *sym_name,
        char *sym_subname
        );

DBGAPI_API
int dbg_initialize_api(
        u_long      access_key,
        dbg_sym_get sym_callback
        );

DBGAPI_API u_long dbg_drv_version();

DBGAPI_API
int dbg_terminate_process(
        IN PVOID  remote_id,
        IN HANDLE proc_id
        );

DBGAPI_API
int dbg_attach_debugger(
        IN PVOID  remote_id,
        IN HANDLE proc_id
        );

DBGAPI_API
HANDLE dbg_create_process(
        IN PVOID remote_id,
        IN PCHAR cmd_line,
        IN ULONG create_flags
        );

DBGAPI_API
int dbg_get_msg_event(
        PVOID    remote_id,
        HANDLE   proc_id, 
        dbg_msg  *msg
        );

DBGAPI_API
int dbg_set_filter(
        PVOID       remote_id,
        HANDLE      proc_id,
        event_filt  *filter
        );

DBGAPI_API
int dbg_countinue_event(
        PVOID             remote_id,
        HANDLE            proc_id, 
        u32               status,
        PEXCEPTION_RECORD new_record
        );

DBGAPI_API
int dbg_read_memory(
        PVOID       remote_id,
        HANDLE      proc_id,
        PVOID       mem_addr,
        PVOID       loc_buff,
        u_long      mem_size,
        u_long      *readed
        );

DBGAPI_API
int dbg_write_memory(
        PVOID       remote_id,
        HANDLE      proc_id,
        PVOID       mem_addr,
        PVOID       loc_buff,
        u_long      mem_size,
        u_long     *written
        );

DBGAPI_API
int dbg_get_context(
        PVOID       remote_id,
        HANDLE      thread_id,
        PCONTEXT    context
        );

DBGAPI_API
int dbg_set_context(
        PVOID       remote_id,
        HANDLE      thread_id,
        PCONTEXT    context
        );

DBGAPI_API
HFILE dbg_open_file(
        IN  LPCSTR lpFileName,
        OUT LPOFSTRUCT lpReOpenBuff,
        IN  UINT uStyle
        );

DBGAPI_API
BOOL dbg_close_file(
        IN  HANDLE hObject
		);

DBGAPI_API
BOOL dbg_read_file(
        IN  HANDLE hFile,
        OUT LPVOID lpBuffer,
        IN  DWORD nNumberOfBytesToRead,
        OUT LPDWORD lpNumberOfBytesRead,
        IN  LPOVERLAPPED lpOverlapped
        );

DBGAPI_API
DWORD dbg_resume_thread(
        IN  HANDLE hThread
        );

DBGAPI_API
DWORD dbg_suspend_thread(
        IN HANDLE hThread               
        );

DBGAPI_API
int dbg_set_rdtsc(
        IN PVOID         context,
        IN BOOLEAN       rdtsc_on
        );

DBGAPI_API
void dbg_close_thread(
        IN PVOID  dbg_thread
        );

DBGAPI_API
PVOID dbg_open_thread(
        IN PVOID        context,
        IN ULONG        thread_id
        );

DBGAPI_API
PVOID dbg_enum_processes(
        IN PVOID remote_id
        );

DBGAPI_API
PVOID dbg_enum_modules(
        IN PVOID context
        );

DBGAPI_API
PVOID dbg_enum_threads(
        IN PVOID remote_id,
        IN ULONG process_id
        );

DBGAPI_API
void dbg_free_memory(
        IN void * memory
        );

DBGAPI_API
int dbg_get_thread_ctx(
        IN  PVOID dbg_thread,
        OUT PVOID trd_context
        );

DBGAPI_API
void dbg_detach_debugger(
        IN PVOID context
        );

DBGAPI_API
int dbg_hook_page(
        IN PVOID context,
        IN PVOID page_addr,
        IN PVOID code_page
        );
