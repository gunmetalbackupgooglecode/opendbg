#define DBGAPI_API extern "C" __declspec(dllimport) 

#define DBGAPI
#pragma pack (push, 1)

typedef struct _OPT_DATA
{
	BOOLEAN anty_format;
	BOOLEAN anty_reboot;
	BOOLEAN prnt_emule;
	BOOLEAN hook_prot;

	BOOLEAN proc_hide;
	BOOLEAN proc_prct;
	BOOLEAN wind_hide;
	BOOLEAN aad_enable;
} OPT_DATA, *POPT_DATA;

typedef struct _PROCESS_INFO
{
	ULONG process_id;
	ULONG parrent_id;
	ULONG num_threads;
	ULONG priority;
	CHAR  exe_path[MAX_PATH];
} PROCESS_INFO, *PPROCESS_INFO;

typedef struct _PROCESS_INFO_EX
{
	ULONG        proc_count;
	PROCESS_INFO processes[];
} PROCESS_INFO_EX, *PPROCESS_INFO_EX;




typedef struct _THREAD_INFO
{
	ULONG thread_id;
	ULONG priority;
	PVOID start_addr;
	PVOID teb_addr;
} THREAD_INFO, *PTHREAD_INFO;

typedef struct _THREAD_INFO_EX
{
	ULONG        thrd_count;
	THREAD_INFO  threads[];
} THREAD_INFO_EX, *PTHREAD_INFO_EX;


typedef struct _MODULE_INFO
{
	PVOID mod_base;
	ULONG mod_size;
	PVOID mod_ep;
	CHAR  mod_file[MAX_PATH];
} MODULE_INFO, *PMODULE_INFO;

typedef struct _MODULE_INFO_EX
{
	ULONG       mod_count;
	MODULE_INFO modules[];
} MODULE_INFO_EX, *PMODULE_INFO_EX;

typedef struct _KTRAP_FRAME 
{
//
//  Following 4 values are only used and defined for DBG systems,
//  but are always allocated to make switching from DBG to non-DBG
//  and back quicker.  They are not DEVL because they have a non-0
//  performance impact.
//

    ULONG   DbgEbp;         // Copy of User EBP set up so KB will work.			// 00
    ULONG   DbgEip;         // EIP of caller to system call, again, for KB.		// 04
    ULONG   DbgArgMark;     // Marker to show no args here.						// 08
    ULONG   DbgArgPointer;  // Pointer to the actual args						// 0C

//
//  Temporary values used when frames are edited.
//
//
//  NOTE:   Any code that want's ESP must materialize it, since it
//          is not stored in the frame for kernel mode callers.
//
//          And code that sets ESP in a KERNEL mode frame, must put
//          the new value in TempEsp, make sure that TempSegCs holds
//          the real SegCs value, and put a special marker value into SegCs.
//

    ULONG   TempSegCs;	// 10
    ULONG   TempEsp;	// 14

//
//  Debug registers.
//

    ULONG   Dr0;	// 18
    ULONG   Dr1;	// 1C
    ULONG   Dr2;	// 20
    ULONG   Dr3;	// 24
    ULONG   Dr6;	// 28
    ULONG   Dr7;	// 2C

//
//  Segment registers
//

    ULONG   SegGs;	//	30
    ULONG   SegEs;	//	34
    ULONG   SegDs;	//	38

//
//  Volatile registers
//

    ULONG   Edx;	//	3C
    ULONG   Ecx;	//	40
    ULONG   Eax;	//	44

//
//  Nesting state, not part of context record
//

    ULONG   PreviousPreviousMode;	//	48

    struct _EXCEPTION_REGISTRATION_RECORD *ExceptionList;	//	4C
                                            // Trash if caller was user mode.
                                            // Saved exception list if caller
                                            // was kernel mode or we're in
                                            // an interrupt.

//
//  FS is TIB/PCR pointer, is here to make save sequence easy
//

    ULONG   SegFs;	//	50

//
//  Non-volatile registers
//

    ULONG   Edi;	//	54
    ULONG   Esi;	//	58
    ULONG   Ebx;	//	5C
    ULONG   Ebp;	//	60

//
//  Control registers
//

    ULONG   ErrCode;	//	64
    UINT_PTR   Eip;		//	68
    ULONG   SegCs;		//	6C
    ULONG   EFlags;		//	70

    ULONG   HardwareEsp;    // WARNING - segSS:esp are only here for stacks	//	74
    ULONG   HardwareSegSs;  // that involve a ring transition.				//	78

    ULONG   V86Es;          // these will be present for all transitions from	//	7C
    ULONG   V86Ds;          // V86 mode											//	80
    ULONG   V86Fs;		//	84
    ULONG   V86Gs;		//	88
} KTRAP_FRAME, *PKTRAP_FRAME;

typedef struct _EXCEPTION_INFO
{
	KTRAP_FRAME       TrapFrame;
	EXCEPTION_RECORD  ExceptionRecord;
	BOOLEAN           FirstChance;
} EXCEPTION_INFO, *PEXCEPTION_INFO;


typedef struct _THREAD_START
{
	ULONG  thread_id;
	PVOID   teb_addr;
	CONTEXT initial_context;
} THREAD_START, *PTHREAD_START;

typedef struct _DLL_LOAD_INFO
{
	PVOID image_base;
	ULONG image_size;
	CHAR  file_name[MAX_PATH];
} DLL_LOAD_INFO, *PDLL_LOAD_INFO;

typedef struct _DLL_UNLOAD_INFO
{
	PVOID image_base;
} DLL_UNLOAD_INFO, *PDLL_UNLOAD_INFO;

typedef struct _THREAD_EXIT
{
	ULONG thread_id;
	ULONG  exit_code;
} THREAD_EXIT, *PTHREAD_EXIT;

typedef struct _CONTINUE
{
	CONTEXT context;
	BOOLEAN alert;
} CONTINUE, *PCONTINUE;

typedef struct _RDTSC_CALL
{
	PVOID addr;
	ULONG edx;
	ULONG eax;	
} RDTSC_CALL, *PRDTSC_CALL;

typedef struct _DBG_EVENT
{
	ULONG process_id;
	ULONG thread_id;
	ULONG  event_code;

	union
	{
		EXCEPTION_INFO  exception;
		THREAD_START    thread_start;
		THREAD_EXIT     thread_exit;
		DLL_LOAD_INFO   dll_load;
		DLL_UNLOAD_INFO dll_unload;
		CONTINUE        seh_continue;
		RDTSC_CALL      rdtsc;
	};

} DBG_EVENT, *PDBG_EVENT;

typedef struct _ATTACH_INFO
{
	PVOID image_base;
	ULONG image_size;
	PVOID image_ep;
	PVOID peb_addr;
	CHAR  file_name[MAX_PATH];
} ATTACH_INFO, *PATTACH_INFO;

typedef
VOID
(*PKNORMAL_ROUTINE) (
    IN PVOID NormalContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

typedef ULONG (_stdcall *PDBGRECV)(PDBG_EVENT dbg_event, void *arg);

typedef struct _DBG_CONTEXT
{
	DBG_EVENT        event_buff;
	HANDLE           dbg_thread;
	HANDLE           dbgd_process;

	HANDLE           exit_event;
	HANDLE           done_event;

	ULONG            status;

	PKNORMAL_ROUTINE apc_callback;
	PDBGRECV         on_event;
	PVOID            user_arg;
} DBG_CONTEXT, *PDBG_CONTEXT;



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

#define FILTER_MAX  100

typedef struct _EXCEPT_FILTER
{
	ULONG  filtr_from;
	ULONG  filtr_to;
} EXCEPT_FILTER, *PEXCEPT_FILTER; 

typedef struct _EVENT_FILTER
{
	ULONG         event_mask;
	ULONG         filtr_count;
	EXCEPT_FILTER filters[FILTER_MAX];
} EVENT_FILTER, *PEVENT_FILTER;

DBGAPI_API int dbg_set_hidden_paths(char *paths, int count);
DBGAPI_API int dbg_get_driver_version(u_short *maj_ver, u_short *min_ver);
DBGAPI_API int dbg_hide_process(HANDLE h_process);
DBGAPI_API int dbg_get_log_msg(char *buff);
DBGAPI_API int dbg_set_options(POPT_DATA options);
DBGAPI_API int dbg_is_proc_hidden(HANDLE proc_id);

DBGAPI_API 
PDBG_CONTEXT dbg_attach_debugger(
				 IN  PVOID        remote_id,
				 IN  ULONG        dbgd_proc_id,
				 IN  PDBGRECV     on_event_proc,
				 IN  PVOID        user_arg,
				 OUT PATTACH_INFO info
				 );

DBGAPI_API
int dbg_set_filter(
	   IN PDBG_CONTEXT  context,
	   IN PEVENT_FILTER filter
	   );

DBGAPI_API
int dbg_set_rdtsc(
	   IN PDBG_CONTEXT  context,
	   IN BOOLEAN       rdtsc_on
	   );

DBGAPI_API
int dbg_hook_page(
	   IN PDBG_CONTEXT  context,
	   IN PVOID         page_addr,
	   IN PVOID         code_page
	   );

DBGAPI_API
int dbg_unhook_page(
	   IN PDBG_CONTEXT  context,
	   IN PVOID         page_addr
	   );

DBGAPI_API
void dbg_detach_debugger(
			IN PDBG_CONTEXT context
			);

DBGAPI_API
int dbg_initialize_api(
	   IN ULONG access_key1,
	   IN ULONG access_key2
	   );

DBGAPI_API
int dbg_read_memory(
		IN PDBG_CONTEXT context,
		IN PVOID        mem_addr,
		IN PVOID        loc_buff,
		IN ULONG        mem_size,
		IN BOOLEAN      code_page
		);

DBGAPI_API
int dbg_write_memory(
		IN PDBG_CONTEXT context,
		IN PVOID        mem_addr,
		IN PVOID        loc_buff,
		IN ULONG        mem_size,
		IN BOOLEAN      code_page
		);


DBGAPI_API
PVOID dbg_open_thread(
	      IN PDBG_CONTEXT context,
		  IN ULONG        thread_id
	  	  );

DBGAPI_API
void dbg_close_thread(
		  IN PVOID  dbg_thread
		  );


DBGAPI_API
int dbg_get_thread_ctx(
	   IN  PVOID        dbg_thread,
	   OUT PCONTEXT     trd_context
	   );


//DBGAPI_API
//int dbg_set_thread_ctx(
//	   IN  PVOID        dbg_thread,
//	   OUT PCONTEXT     trd_context
//	   );

DBGAPI_API
void dbg_suspend_thread(
		  IN PVOID dbg_thread
		  );

DBGAPI_API
void dbg_resume_thread(
		  IN PVOID dbg_thread
		  );


DBGAPI_API
PVOID dbg_open_file(
		IN PDBG_CONTEXT context,
		IN PCHAR        file_name,
		IN ULONG        need_access,
		IN ULONG        open_disposition
		);


DBGAPI_API
void dbg_close_file(
	    IN PVOID dbg_file
		);


DBGAPI_API
ULONG dbg_get_file_size(
		  IN PVOID dbg_file
		  );


DBGAPI_API
int dbg_read_file(
		IN PVOID dbg_file,
		IN PVOID buff,
		IN ULONG size
		);


DBGAPI_API
int dbg_write_file(
		IN PVOID dbg_file,
		IN PVOID buff,
		IN ULONG size
		);


DBGAPI_API
void dbg_seek_file(
	    IN PVOID dbg_file,
		IN ULONG new_pos,
		IN ULONG method
		);


DBGAPI_API
int dbg_create_process(
		 IN PVOID remote_id,
		 IN PCHAR cmd_line,
		 IN ULONG create_flags
		 );

DBGAPI_API
int dbg_terminate_process(
		  IN PVOID remote_id,
		  IN ULONG proc_id
		  );

DBGAPI_API
PPROCESS_INFO_EX 
     dbg_enum_processes(
           IN PVOID remote_id
		   );

DBGAPI_API
PTHREAD_INFO_EX
     dbg_enum_threads(
           IN PVOID remote_id,
		   IN ULONG process_id
		   );

DBGAPI_API
PMODULE_INFO_EX
    dbg_enum_modules(
	   IN PDBG_CONTEXT context
	   );

DBGAPI_API
void dbg_free_memory(void *memory);

#define dbg_write_memory(q,w,e,r,t) (OutputDebugString("=============writemem====="), dbg_write_memory(q,w,e,r,t))