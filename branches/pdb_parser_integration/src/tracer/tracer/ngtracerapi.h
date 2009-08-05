
#define TRACERAPI	extern "C" __declspec(dllexport)


#ifndef DBGAPI_API
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

#endif //DBGAPI_API


typedef struct _trcrf {

 ULONG   Dr0;
 ULONG   Dr1;
 ULONG   Dr2;
 ULONG   Dr3;
 ULONG   Dr6;	//	24 from TRC_EXCEPT
 ULONG   Dr7;

 ULONG   SegGs;
 ULONG   SegEs;
 ULONG   SegDs;

 ULONG   Edx;
 ULONG   Ecx;
 ULONG   Eax;
 ULONG   SegFs;

 ULONG   Edi;
 ULONG   Esi;
 ULONG   Ebx;
 ULONG   Ebp;

 UINT_PTR   Eip;
 ULONG		SegCs;
 ULONG		EFlags;

 ULONG	Esp;
 ULONG	SegSs;

} TRC_REG_FRAME,*PTRC_REG_FRAME;

typedef struct _trcmod {
PVOID ImageBase;
ULONG ImageSize;
CHAR  ModName[MAX_PATH];
} TRC_MODULE,*PTRC_MODULE;

typedef struct _trcthr {
ULONG	TID;
union
{
PVOID	teb_addr;
ULONG	exit_code;
};
} TRC_THREAD,*PTRC_THREAD;

typedef struct _trcprc {
ULONG	PID;
PVOID	peb_addr;
} TRC_PROCESS,*PTRC_PROCESS;


typedef struct _proc_evt {
ULONG			EventCode;
union
{
PTRC_THREAD		StartedThread;
PTRC_THREAD		ExitedThread;
PTRC_MODULE		LoadedModule;
PTRC_MODULE		UnloadedModule;
PTRC_PROCESS	TerminatedProcess;
};

} TRC_PROCESS_EVENT,*PTRC_PROCESS_EVENT;

typedef struct _dbg_evt {
ULONG			EventCode;
TRC_REG_FRAME	Frame;
PTRC_PROCESS	CurrentProcess;
PTRC_THREAD		CurrentThread;
PTRC_MODULE		CurrentModule;
} TRC_EXCEPTION_EVENT,*PTRC_EXCEPTION_EVENT;



typedef struct _bp{
	ULONG		TID;
	ULONG		type;
	PVOID		addr;
	ULONG		range;
}BP,*PBP;

typedef struct _bplst{
	ULONG	count;
	BP		bp[1];
}BP_LST,*PBP_LST;

typedef struct _SEH_INFO {
	PVOID	frame_addr;	// 00
	PVOID	handler;	// 04
} SEH_INFO, *PSEH_INFO;

typedef struct _SEH_LIST {
	ULONG 		count; // 00
	SEH_INFO 	seh[]; // 04
} SEH_LIST, *PSEH_LIST;

typedef	struct _modlst{
	ULONG		count;
	TRC_MODULE	module[1];
}MOD_LIST,*PMOD_LIST;

typedef	struct	_thrdlst{
	ULONG		count;
	TRC_THREAD	thread[1];
}THREAD_LIST,*PTHREAD_LIST;

typedef ULONG (_stdcall *PTRC_EXCPT_EVT)(PTRC_EXCEPTION_EVENT excpt_event, void *arg);
typedef ULONG (_stdcall *PTRC_PROC_EVT)(PTRC_PROCESS_EVENT proc_event, void *arg);

TRACERAPI	int					trc_init();
TRACERAPI	PPROCESS_INFO_EX	trc_enum_processes(ULONG sesId);
TRACERAPI	PTHREAD_INFO_EX		trc_enum_threads(ULONG sesId,ULONG PID);
TRACERAPI	ULONG				trc_read_memory(ULONG sesId,PVOID addr,PVOID buffer,ULONG size);
TRACERAPI	ULONG				trc_write_memory(ULONG sesId,PVOID addr,PVOID buffer,ULONG size);
TRACERAPI	int					trc_session_open(ULONG sessionType,LPVOID sessionParams);
TRACERAPI   VOID				trc_session_close(ULONG sesId);

TRACERAPI   ULONG				trc_set_bp(ULONG sesId,ULONG bp_type,ULONG TID,PVOID addr,ULONG range);
TRACERAPI	ULONG				trc_delete_bp(ULONG sesId,ULONG TID,PVOID addr, ULONG bp_type_mask);
TRACERAPI	PBP_LST				trc_get_bp_list(ULONG sesId,ULONG TID);
TRACERAPI	void				trc_enable_bp(ULONG SessId, ULONG bp_type, PVOID addr, ULONG TID, BOOL bEnable);
TRACERAPI	PBP_LST				trc_get_disabled_bp_list(ULONG SessId, ULONG TID);
TRACERAPI	VOID				trc_free_list(PVOID pbp_list);

TRACERAPI   ULONG				trc_set_options(ULONG sesId,ULONG opt);
TRACERAPI   ULONG				trc_get_options(ULONG sesId);
TRACERAPI	void				trc_free_mem(LPVOID mem);
TRACERAPI	ULONG				trc_load_target(ULONG sesId,PCHAR imgPath,ULONG loadOpts);
TRACERAPI	ULONG				trc_set_callback(ULONG sesId,ULONG type,PVOID callback,PVOID arg);
TRACERAPI	ULONG				trc_get_thread_ctx(ULONG sesId,ULONG TID,PCONTEXT ctx);
TRACERAPI	ULONG				trc_set_thread_ctx(ULONG sesId,ULONG TID,PCONTEXT ctx);
TRACERAPI	ULONG				trc_attach_to_target(ULONG sesId,ULONG PID,ULONG attachOpts);

TRACERAPI	PMOD_LIST 			trc_get_module_list(ULONG	sesId);
TRACERAPI	PTHREAD_LIST		trc_get_thread_list(ULONG	sesId);
TRACERAPI PSEH_LIST trc_get_seh_chain(ULONG SessId,ULONG tid);

TRACERAPI	ULONG				trc_suspend_thread(ULONG sesId, ULONG TID);
TRACERAPI	ULONG				trc_resume_thread(ULONG sesId, ULONG TID);
TRACERAPI	bool				trc_break_on_thread(ULONG SessId, ULONG tid);
TRACERAPI	ULONG				trc_terminate_process(ULONG sesId, ULONG PID);


#define		TRC_SESSION_LOCAL	1
#define		TRC_SESSION_INVALID	-1

#define		TRC_OPT_BREAK_ON_ATTACH				1
#define		TRC_OPT_BREAK_ON_EP					2
#define		TRC_OPT_BREAK_ON_MOD_EP				4
#define		TRC_OPT_BREAK_ON_SEH_HANDLER		8
#define		TRC_OPT_HIDE_BREAKPOINTS			16
#define		TRC_OPT_BREAK_ON_TLS				32
#define		TRC_OPT_BREAK_ON_NEW_THREAD			64
#define		TRC_OPT_BREAK_ON_KI_USER_EX_DISP	128
#define		TRC_OPT_PROTECT_DRS					256
#define		TRC_OPT_PROTECT_TF					512
#define		TRC_OPT_CONTINUE_TRACE_AFTER_SEH	1024
#define		TRC_OPT_ONCE_BREAK					2048



#define		TRC_BP_SOFTWARE				1
#define		TRC_HWBP_READWRITE			2
#define		TRC_HWBP_WRITE				4
#define		TRC_HWBP_EXECUTE			8
#define		TRC_HWBP_IO					16
#define		TRC_BP_ONE_SHOT				32

#define		TRC_DBG_EVENTS_CALLBACK				1
#define		TRC_PROCESS_EVENTS_CALLBACK			2
#define		TRC_EXCEPTIONS_CALLBACK				4

//debug events
#define		TRC_EVENT_BREAKPOINT		1
#define		TRC_EVENT_SINGLE_STEP		2
//process events
#define		TRC_EVENT_TERMINATE			3
#define		TRC_EVENT_THREAD_START		4
#define		TRC_EVENT_THREAD_EXIT		5
#define		TRC_EVENT_LOAD_MODULE		6
#define		TRC_EVENT_UNLOAD_MODULE		7
//exceptions


#define		TRC_CORECT_FRAME			1
#define		TRC_EXC_HANDLE				2
#define		TRC_EXC_NOT_HANDLE			4

#define		TRC_RUN						1
#define		TRC_STEP_INTO				2
#define		TRC_STEP_OVER				4
#define		TRC_RUN_TIL_RET				8
#define		TRC_STEALTH_TRACE			16

