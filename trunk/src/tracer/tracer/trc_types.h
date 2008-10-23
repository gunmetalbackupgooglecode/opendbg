#include "windows.h"
#include "dbgapi.h"
#include "ngtracerapi.h"

#define ALLOC(x) (P##x)HeapAlloc(hHeap, 0, sizeof(x))
#define FREE(x)	 HeapFree(hHeap, 0, x)

typedef struct _EXCEPTION_REGISTRATION {
	struct _EXCEPTION_REGISTRATION* prev;		// 00
	PVOID handler;	// 04
} EXCEPTION_REGISTRATION, *PEXCEPTION_REGISTRATION;


typedef struct _MODULE_LIST {
	struct _MODULE_LIST* pNext;	
	TRC_MODULE data;
	BYTE	by_118;	//	118h
	BYTE	ep_bp_break;	//not reads?...
	BYTE	bFirstBreak;	// 11Ah	-	only for debug?)
} MODULE_LIST, *PMODULE_LIST;

typedef struct _drs {
	ULONG	dr0;
	ULONG	dr1;
	ULONG	dr2;
	ULONG	dr3;
	ULONG	dr6;
	ULONG	dr7;
} DRS, *PDRS;


typedef struct _EXCEP_FRAME {
	struct _EXCEP_FRAME* pPrev; //pointer to the previously raised exception frame
	ULONG	CallRes;
	BYTE	TraceMode;
	BOOL	bDebugTraced;
	PSEH_LIST pSehs;	//zero if no seh chains =)	
} EXCEP_FRAME, *PEXCEP_FRAME;

typedef struct _THREAD_DATA {
	TRC_THREAD	trc_thrd;	//00
	HANDLE			dbg_handle;	//	08
	ULONG		dw_12D;
	struct {	//	18
		ULONG		bEnabled;	// 10 + 10*x
		PVOID		addr;		// 14 + 10*x
		ULONG		type;		// 18 + 10*x
		ULONG		range;		// 1C + 10*x		
	} breakpoint[4];
	PVOID	EnableDrAddr;
	BYTE	UnchangedContext;				//	50
	//BOOL	LastCallbackRet;

	BOOL	bNeedCopyDr;
	DRS		DrDbg;
	DRS		DrProc;

	BYTE	bDbgTraced;
	BYTE	bProgTrace;
	BYTE	bNextTraceCmd;
	PVOID	TraceAddr;
	BYTE	TraceMode;

	PTRC_REG_FRAME	pFastContext;
	PEXCEP_FRAME	pExcFrames; // pointer to except. handlers

} THREAD_DATA, *PTHREAD_DATA;

typedef struct _THREAD_DATA_EX {
	struct _THREAD_DATA_EX* 	pNext;	// 00
	//struct _THREAD_DATA_EX*		pPrev;	// 04
	THREAD_DATA					data;	// 08
} THREAD_DATA_EX, *PTHREAD_DATA_EX;

typedef struct _LST {
	struct _LST* pNext;
	PVOID value;
} LST, *PLST;



typedef struct _CALLBACK_INFO {
	LPVOID	lpCallback;		// 00
	PVOID	arg;			// 04	
} CALLBACK_INFO, *PCALLBACK_INFO;



#define TRC_BP_USER		1

#define TRC_BP_THREAD	4
#define TRC_BP_TIL_RET	8
#define TRC_BP_SHADOW	16

//#define TRC_BP_ONE_SHOT	32


typedef struct _TID_EX {
	struct _TID_EX* pNext;
	ULONG tid;
} TID_EX, *PTID_EX;

typedef struct _BREAKPOINT_LIST {
	struct _BREAKPOINT_LIST* pNext; // 00
		
	CHAR  	SavedByte;      // 0C	
	LPVOID 	pHookedMem;		// 13	

	PVOID 	addr;			// 04	
	CHAR	Kind;
	PTID_EX	pTidList;	
	PTID_EX pTilRetTidList;
	PTID_EX pShadowTidList;
	ULONG	event_code;
} BREAKPOINT_LIST, *PBREAKPOINT_LIST;

typedef struct _BP_EX {
	struct _BP_EX* pNext;
	BP	data;
} BP_EX, *PBP_EX;

typedef struct _SESSION_INFO {
	ULONG 		SessionId;  // 00
	ULONG		bTerminated;  // 04
	UINT_PTR	remote_id;  // 08
	ULONG       options;    // 0C
	CALLBACK_INFO DbgEvent; // 10    
	CALLBACK_INFO ProcEvent;// 18    
	CALLBACK_INFO ExcEvent; // 20    
	PTHREAD_DATA_EX pThreads;  // 28 - threads?
	PMODULE_LIST pModules;  // 2C - modules?
	PBREAKPOINT_LIST pSoftBps; // 30
	PBP_EX		pDisabledBps;
	PDBG_CONTEXT pDbgContext;// 34	
	CRITICAL_SECTION	SyncCS;
	char		image_name[265];
	PVOID	CC_need_addr;
	TRC_PROCESS	trc_process;
} SESSION_INFO, *PSESSION_INFO;

typedef struct _SESSION_INFO_EX {
	struct _SESSION_INFO_EX * pNext;
	SESSION_INFO data;
} SESSION_INFO_EX, *PSESSION_INFO_EX;

/*
typedef struct _PROCESS_TMP_DATA {
	ULONG	mod_size;
	LPVOID	mod_base;
	char	str[261];
} PROCESS_TMP_DATA, *PPROCESS_TMP_DATA;
*/



///////////////////////////////////////////////////////
// global variables

extern SESSION_INFO_EX* lpFirstSessListItem;


extern LPVOID hHeap;
extern BOOL by_is_inited;


//////////////////////////////////////////////////////////////////////////
// declarations

extern PBP_LST get_soft_bp_lst(ULONG SessId);
extern PBP_LST get_hard_bp_lst(ULONG SessId, ULONG tid);
extern PBP_LST get_global_hws(PSESSION_INFO pSessData);
extern PBP_LST  get_local_hws(PTHREAD_DATA pBps);
extern ULONG DbgEv2ExcEv(PDBG_EVENT pDbgEvent, PTRC_EXCEPTION_EVENT a2);
extern PBREAKPOINT_LIST   find_soft_bp_by_addr(PBREAKPOINT_LIST pBps, PVOID addr);
extern ULONG __stdcall OnEventProc(PDBG_EVENT pDbgEvent, ULONG SessId);
//ULONG __stdcall OnEventProc(PDBG_EVENT pDbg,ULONG SessId)

extern void create_threads(SESSION_INFO* pSessData, PTHREAD_INFO_EX pThreadInfoEx);

MODULE_LIST* __cdecl add_module(PSESSION_INFO pSessData, PDLL_LOAD_INFO pSmth);

extern PTHREAD_DATA_EX __cdecl add_thread(PSESSION_INFO pSessData, PTHREAD_START pSmth);
extern ULONG __cdecl delete_thread(PSESSION_INFO pSessData, PTHREAD_EXIT a2);

extern PTHREAD_DATA __cdecl get_thrd_by_tid(PSESSION_INFO pSessDataStruct, ULONG tid);
extern ULONG create_modules(PSESSION_INFO pSessData, PMODULE_INFO_EX pModuleInfoEx);
extern PMODULE_LIST get_module_by_base(PSESSION_INFO a1, PVOID a2);
extern signed long  delete_module(PSESSION_INFO pSessData, PDLL_UNLOAD_INFO a2);

//extern ULONG __cdecl SetSoftwareBreakpoint(ULONG SessId,PVOID addr);
//extern ULONG set_soft_bp(ULONG SessId,PVOID addr);
//extern ULONG __cdecl SetHardwareBreakpoint(ULONG SessId, ULONG tid, int bp_type, ULONG range, ULONG addr);
//extern ULONG set_hard_bp(ULONG SessId, ULONG tid, int bp_type, ULONG range, PVOID addr);
//extern ULONG set_thread_hw(int SessId, int tid, int bp_type, int range, PVOID addr);
//extern ULONG set_thread_hw(ULONG SessionId,ULONG TID, signed long bp_type, ULONG range, PVOID addr);
//extern ULONG delete_thrd_hw(int SesId,int tid, PVOID addr, ULONG bp_type_mask);
//extern ULONG do_delete_soft_bp(ULONG SessId, PVOID addr);
//extern ULONG delete_hw(int Sesid, int tid, PVOID addr, ULONG bp_type_mask);
extern ULONG read_memory(ULONG sesId,PVOID addr,PVOID buffer,ULONG size,BOOLEAN code_page);
extern ULONG write_memory(ULONG sesId,PVOID addr,PVOID buffer,ULONG size,BOOLEAN code_page);

signed long ProcessException(PDBG_EVENT pDbg, PSESSION_INFO pSessData, PTRC_EXCEPTION_EVENT pSomeData);

PSESSION_INFO get_sess_by_id(ULONG SessId);
//ULONG trc_set_bp_ex(ULONG SessId, PVOID addr, ULONG count_to_break, BOOL bEngineBp, BOOL bTraced);
//ULONG trc_set_bp_ex(ULONG SessId, PVOID addr, ULONG count_to_break, ULONG bEngineBp, ULONG bTraced);
//BOOL restore_bp(PSESSION_INFO pSessData);
void restore_bp(PSESSION_INFO pSessData, PTHREAD_DATA pThrd, PVOID addr);

VOID set_trace_into(PSESSION_INFO pSess, PTRC_EXCEPTION_EVENT pExcepEvent, PDBG_EVENT pDbgEvent);

bool set_bp_user(PSESSION_INFO pSess, PVOID addr);
bool set_bp_one_shot(PSESSION_INFO pSess, PVOID addr);
bool set_bp_tid(PSESSION_INFO pSess, PVOID addr, ULONG TID);
//bool set_bp_trace(PSESSION_INFO pSess, PVOID addr, ULONG TID);
bool search_tid(PTID_EX pBp, ULONG TID );
void delete_bp_user(PSESSION_INFO pSess, PVOID addr);
//void delte_bp_by_tid(PSESSION_INFO pSess, PVOID addr, ULONG TID);
//void delete_bp_one_shot(PSESSION_INFO pSess, PVOID addr);
//void delete_bp_trace(PSESSION_INFO pSess, PVOID addr, ULONG TID);
bool set_bp_ex(PSESSION_INFO pSess, PVOID addr, ULONG kind, ULONG tid, ULONG event_code);
ULONG process_bp_break(PSESSION_INFO pSess, PVOID addr, ULONG tid, PULONG lpEvCode);
void delete_bp(PSESSION_INFO pSess, PVOID addr, ULONG tid, ULONG type_mask);

void delete_all_modules(PSESSION_INFO pSess);
void delete_all_threads(PSESSION_INFO pSess);
void delete_all_breakpoints(PSESSION_INFO pSess);
PEXCEP_FRAME create_except_frame(PSESSION_INFO pSess, PTHREAD_DATA pThread);

int TrcFrame2Con(PCONTEXT ctx, PTRC_REG_FRAME frm);
int Con2TrcFrame(PTRC_REG_FRAME frm, PCONTEXT ctx);

bool IsChainCmd(PUCHAR buff);
bool IsUntraceableCmd(PUCHAR pBuffer);
bool IsTraceOverCmd(PSESSION_INFO pSess, PVOID addr);
int Get_code_size(PSESSION_INFO pSess, PVOID addr);
DWORD get_ep_rva(PVOID pModBase);