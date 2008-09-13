#define DBGAPI_API __declspec(dllexport) 

#include "..\sys\dbg_def.h"

#define SYM_TIMESTAMP     0 /* timestamp of kernel image */
#define SYM_NTAPI_NUM     1 /* number of Nt api          */
#define SYM_OFFSET        2 /* global symbol offset      */
#define SYM_STRUCT_OFFSET 3 /* structure field offset    */

typedef u_long (CALLBACK *dbg_sym_get)(
				int sym_type, char *sym_name, char *sym_subname
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
		dbg_msg *msg
		);

DBGAPI_API
int dbg_set_filter(
	   PVOID       remote_id,
	   HANDLE      proc_id,
	   event_filt *filter
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
	   u_long     *readed
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