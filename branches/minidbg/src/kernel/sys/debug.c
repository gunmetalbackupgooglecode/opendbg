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

#include <ntifs.h>
#include "defines.h"
#include "debug.h"
#include "dbg_item.h"
#include "syscall.h"
#include "driver.h"
#include "extdef.h"
#include "regio.h"
#include "sdt_hook.h"
#include "dbg_def.h"
#include "misc.h"
#include "code_hook.h"

static PZWTERMINATEPROCESS  NtTerminateProcess;
static PZWRESUMETHREAD      NtResumeThread;
static PZWTERMINATEPROCESS  OldZwTerminateProcess;
static PZWCREATETHREAD      OldZwCreateThread;
static PZWTERMINATETHREAD   OldZwTerminateThread;
static PKIDISPATCHEXCEPTION OldKiDispatchException;
static u32                  thread_enth_offs;

void dbg_process_terminated(
		PEPROCESS process, NTSTATUS exit_code
		);

typedef struct _term_event {
	dbg_item *debug;
	dbg_msg   msg;

} term_event;

int dbg_open_process(syscall *data)
{
	PEPROCESS process;
	NTSTATUS  status;
	HANDLE    h_proc;
	int       succs = 0;

	if ( (data->in_size  == sizeof(HANDLE)) &&
		 (data->out_size == sizeof(HANDLE)) )
	{
		status = PsLookupProcessByProcessId(
			        *(HANDLE*)(data->in_data), &process
					);

		if (NT_SUCCESS(status))
		{
			if (h_proc = open_process_by_ptr(process, 0))
			{
				*(HANDLE*)(data->out_data) = h_proc;
				succs = 1;
			}

			ObDereferenceObject(process);
		}
	}

	return succs;
}

int dbg_terminate_process(syscall *data)
{
	NTSTATUS  status;
	PEPROCESS process;
	HANDLE    h_proc;
	int       succs = 0;

	if (data->in_size == sizeof(HANDLE))
	{
		h_proc = *(HANDLE*)(data->in_data);

		status = PsLookupProcessByProcessId(
			h_proc, &process
			);

		if (NT_SUCCESS(status))
		{
			dbg_process_terminated(
				process, STATUS_SUCCESS
				);

			ObDereferenceObject(process);
		}

		status = NtTerminateProcess(
			h_proc, STATUS_SUCCESS
			);

		if (NT_SUCCESS(status)) {
			succs = 1;
		}
	}

	return succs;
}

int dbg_attach_call(syscall *data)
{
	PEPROCESS process = NULL;
	dbg_item *debug   = NULL;
	int       succs   = 0;
	channel  *chan;
	NTSTATUS  status;

	do
	{
		if (data->in_size != sizeof(HANDLE)) {
			break;
		}

		status = PsLookupProcessByProcessId(
			        *(HANDLE*)(data->in_data), &process
					);

		if (NT_SUCCESS(status) == FALSE) {
			break;
		}

		if (process == IoGetCurrentProcess()) {
			break;
		}

		if (debug = dbg_find_item(NULL, process)) {
			break;
		}

		if ( (chan = channel_alloc(sizeof(dbg_msg), sizeof(cont_dat))) == NULL ) {
			break;
		}

		if ( (debug = dbg_new_item(process)) == NULL ) {
			break;
		}

		debug->chan   = chan;
		debug->active = 1;
		succs = 1;
	} while (0);

	if (debug != NULL) {
		dbg_deref_item(debug);
	}

	if (process != NULL) {
		ObDereferenceObject(process);
	}

	if (succs == 0)
	{
		if (chan != NULL) {
			channel_free(chan);
		}
	}

	return succs;
}

int dbg_set_filter(syscall *data)
{
	//PEPROCESS        process = NULL;
	//int              succs   = 0;
	//dbg_item        *debug   = NULL;
	//set_filter_data *filter  = data->in_data;
	//NTSTATUS         status;

	//do
	//{
	//	if (data->in_size != sizeof(set_filter_data)) {
	//		break;
	//	}

	//	status = PsLookupProcessByProcessId(
	//		        filter->process, &process
	//				);

	//	if (NT_SUCCESS(status) == FALSE) {
	//		break;
	//	}

	//	if ( (debug = dbg_find_item(IoGetCurrentProcess(), process)) == NULL ) {
	//		break;
	//	}

	//	memcpy(
	//		&debug->filter, &filter->filter, sizeof(event_filt)
	//		);

	//	succs = 1;
	//} while (0);

	//if (debug != NULL) {
	//	dbg_deref_item(debug);
	//}

	//if (process != NULL) {
	//	ObDereferenceObject(process);
	//}

	//return succs = 1;
	return 1;
}

int dbg_get_message(syscall *data)
{
	PEPROCESS process = NULL;
	int       succs   = 0;
	dbg_item *debug   = NULL;
	NTSTATUS  status;

	do
	{
		if ( (data->in_size != sizeof(HANDLE)) || (data->out_size != sizeof(dbg_msg)) ) {
			break;
		}

		status = PsLookupProcessByProcessId(
			*(HANDLE*)(data->in_data), &process
			);

		if (NT_SUCCESS(status) == FALSE) {
			break;
		}

		if ( (debug = dbg_find_item(IoGetCurrentProcess(), process)) == NULL ) {
			break;
		}

		succs = channel_recv(
			debug->chan, data->out_data
			);
	} while (0);

	if (process != NULL) {
		ObDereferenceObject(process);
	}

	if (debug != NULL) {
		dbg_deref_item(debug);
	}

	return succs;
}

static
NTSTATUS
  NewZwTerminateProcess(
    HANDLE   h_process,
    NTSTATUS exit_status
    )
{
	NTSTATUS  status;
	PEPROCESS process;
	HANDLE    h_proc;

	if (h_process == NULL) {
		h_proc = NtCurrentProcess();
	} else {
		h_proc = h_process;
	}

	if (ExGetPreviousMode() == UserMode)
	{
		status = ObReferenceObjectByHandle(
			h_proc, 0x0001, *PsProcessType, UserMode, &process, NULL
			);

		if (NT_SUCCESS(status))
		{
			dbg_process_terminated(
				process, exit_status
				);

			ObDereferenceObject(process);
		}
	}

	return OldZwTerminateProcess(h_proc, exit_status);
}



static
void dbg_delayed_term_event(term_event *t_event)
{
	cont_dat cont;

	DbgMsg("dbg_delayed_term_event\n");

	channel_send_recv(
		t_event->debug->chan, &t_event->msg, &cont
		);

	dbg_delete_item(t_event->debug);
	dbg_deref_item(t_event->debug);

	mem_free(t_event);
}

static
void dbg_process_terminated(
		PEPROCESS process, NTSTATUS exit_code
		)
{
	HANDLE      h_proc;
	dbg_item   *debug;
	term_event *t_event;
	int         succs;

	/* find all processes debugged by terminated */
	while (debug = dbg_find_item(process, NULL))
	{
		/* terminate debugged process */
		if (h_proc = open_process_by_ptr(debug->debugged, 0))
		{
			NtTerminateProcess(
				h_proc, STATUS_SUCCESS
				);

			ZwClose(h_proc);
		}

		dbg_delete_item(debug);
		dbg_deref_item(debug);
	}

	/* find terminated debugged process */
	if (debug = dbg_find_item(NULL, process))
	{
		t_event = NULL; succs = 0;

		do
		{
			if ( (debug->filter.event_mask & DBG_TERMINATED) == 0 ) {
				break;
			}

			if ( (t_event = mem_alloc(sizeof(term_event))) == NULL ) {
				break;
			}

			t_event->debug          = debug;
			t_event->msg.process_id = PsGetCurrentProcessId();
			t_event->msg.thread_id  = PsGetCurrentThreadId();
			t_event->msg.event_code = DBG_TERMINATED;

			t_event->msg.terminated.exit_code = exit_code;
			t_event->msg.terminated.proc_id   = PsGetProcessId(process);

			succs = start_system_thread(
				dbg_delayed_term_event, t_event
				);

		} while (0);

		if (succs == 0)
		{
			if (t_event != NULL) {
				mem_free(t_event);
			}

			dbg_deref_item(debug);
		}
	}
}

static
NTSTATUS
  NewZwCreateThread(
     PHANDLE            ThreadHandle,
	 ACCESS_MASK        DesiredAccess,
	 POBJECT_ATTRIBUTES ObjectAttributes,
	 HANDLE             ProcessHandle,
	 PCLIENT_ID         ClientId,
	 PCONTEXT           ThreadContext,
	 PVOID              UserStack,
	 BOOLEAN            CreateSuspended
	 )
{
	THREAD_BASIC_INFORMATION info;
	PEPROCESS                process = NULL;
	dbg_item                *debug   = NULL;
	NTSTATUS                 status;
	NTSTATUS                 statu2;
	dbg_msg                  msg;
	cont_dat                 cont;
	HANDLE                   h_thread;

	status = OldZwCreateThread(
		ThreadHandle, DesiredAccess,
		ObjectAttributes, ProcessHandle,
		ClientId, ThreadContext, UserStack, TRUE
		);

	if (NT_SUCCESS(status))
	{
		do
		{
			__try {
				h_thread = *ThreadHandle;
			}
			__except(EXCEPTION_EXECUTE_HANDLER) {
				h_thread = NULL;
				break;
			}

			statu2 = ObReferenceObjectByHandle(
				ProcessHandle, 0, *PsProcessType,
				UserMode, &process, NULL
				);

			if (NT_SUCCESS(statu2) == FALSE) {
				break;
			}

			if ( (debug = dbg_find_item(NULL, process)) == NULL ) {
				break;
			}

			if ( (debug->filter.event_mask & DBG_START_THREAD) == 0 ) {
				break;
			}

			zeromem(&info, sizeof(info));
			zeromem(&msg,  sizeof(msg));

			ZwQueryInformationThread(
				h_thread, ThreadBasicInformation,
				&info, sizeof(info), NULL
				);

			msg.process_id = PsGetCurrentProcessId();
			msg.thread_id  = PsGetCurrentThreadId();
			msg.event_code = DBG_START_THREAD;

			__try
			{
				msg.thread_start.thread_id = info.ClientId.UniqueThread;
				msg.thread_start.teb_addr  = info.TebBaseAddress;

				memcpy(
					&msg.thread_start.initial_context, ThreadContext, sizeof(CONTEXT)
					);
			}
			__except(EXCEPTION_EXECUTE_HANDLER) {
				break;
			}

			/* send debug event and wait replay */
			if (channel_send_recv(debug->chan, &msg, &cont) == 0) {
				break;
			}
		} while (0);

		if (process != NULL) {
			ObDereferenceObject(process);
		}

		if (debug != NULL) {
			dbg_deref_item(debug);
		}

		if ( (h_thread != NULL) && (CreateSuspended == FALSE) )
		{
			NtResumeThread(
				h_thread, NULL
				);
		}
	}

	return status;
}

static
NTSTATUS
  NewZwTerminateThread(
     HANDLE   ThreadHandle,
	 NTSTATUS ExitStatus
	 )
{
	HANDLE      h_thread;
	PETHREAD    thread = NULL;
	dbg_item   *debug  = NULL;
	PEPROCESS   process;
	NTSTATUS    status;
	dbg_msg     msg;
	cont_dat    cont;
	PLIST_ENTRY thr_list;

	if (ThreadHandle == NULL) {
		h_thread = NtCurrentThread();
	} else {
		h_thread = ThreadHandle;
	}

	do
	{
		status = ObReferenceObjectByHandle(
			h_thread, THREAD_TERMINATE,
		   *PsThreadType, ExGetPreviousMode(),
		    &thread, NULL
			);

		if (NT_SUCCESS(status) == FALSE) {
			break;
		}

		process = IoThreadToProcess(thread);

		if ( (debug = dbg_find_item(NULL, process)) == NULL ) {
			break;
		}

		if ( (debug->filter.event_mask & DBG_EXIT_THREAD) == 0 ) {
			break;
		}

		msg.process_id = PsGetCurrentProcessId();
		msg.thread_id  = PsGetCurrentThreadId();
		msg.event_code = DBG_EXIT_THREAD;

		msg.thread_exit.thread_id = PsGetThreadId(thread);
		msg.thread_exit.proc_id   = PsGetProcessId(process);
		msg.thread_exit.exit_code = ExitStatus;

		/* send debug event and wait replay */
		if (channel_send_recv(debug->chan, &msg, &cont) == 0) {
			break;
		}
	} while (0);

	if (thread != NULL)
	{
		/* report for terminate process if last thread terminated */
		thr_list = addof(thread, thread_enth_offs);

		if (thr_list->Flink->Flink == thr_list)
		{
			dbg_process_terminated(
				process, ExitStatus
				);
		}

		ObDereferenceObject(thread);
	}

	if (debug != NULL) {
		dbg_deref_item(debug);
	}

	return OldZwTerminateThread(h_thread, ExitStatus);
}

static
int dbg_process_exception(
	   PEXCEPTION_RECORD except_record,
	   BOOLEAN           first_chance
	   )
{
	dbg_item *debug   = NULL;
	int       handled = 0;
	u32       i, code = except_record->ExceptionCode;
	dbg_msg   msg;
	cont_dat  cont;

	do
	{
		if ( (debug = dbg_find_item(NULL, IoGetCurrentProcess())) == NULL ) {
			break;
		}

		/* check event mask */
		if ( (debug->filter.event_mask & DBG_EXCEPTION) == 0 ) {
			break;
		}

		/* check exception filters */
		for (i = 0; i < debug->filter.filtr_count; i++)
		{
			if ( (code >= debug->filter.filters[i].filtr_from) &&
				 (code <= debug->filter.filters[i].filtr_to) )
			{
				break;
			}
		}

		/* setup debug message */
		msg.process_id             = PsGetCurrentProcessId();
		msg.thread_id              = PsGetCurrentThreadId();
		msg.event_code             = DBG_EXCEPTION;
		msg.exception.first_chance = first_chance;

		/* copy exception record */
		fastcpy(
			&msg.exception.except_record, except_record, sizeof(EXCEPTION_RECORD)
			);

		/* send debug message and recive replay */
		if (channel_send_recv(debug->chan, &msg, &cont) == 0) {
			break;
		}

		if (cont.status & RES_CORRECT_FRAME)
		{
			/* correct exception record */
			fastcpy(
				except_record, &cont.new_record, sizeof(EXCEPTION_RECORD)
				);
		}

		if (cont.status & RES_CONTINUE) {
			handled = 1;
		}
	} while (0);

	if (debug != NULL) {
		dbg_deref_item(debug);
	}

	return handled;
}

static void
  NewKiDispatchException(
    PEXCEPTION_RECORD ExceptionRecord,
    PKEXCEPTION_FRAME ExceptionFrame,
    PKTRAP_FRAME      TrapFrame,
    KPROCESSOR_MODE   PreviousMode,
    BOOLEAN           FirstChance
    )
{
	if (PreviousMode == UserMode)
	{
		if (dbg_process_exception(ExceptionRecord, FirstChance) != 0) {
			return;
		}
	}

	OldKiDispatchException(
		ExceptionRecord, ExceptionFrame, TrapFrame,
		PreviousMode, FirstChance
		);
}

int dbg_continue(syscall *data)
{
	cont_dat *msg     = data->in_data;
	PEPROCESS process = NULL;
	dbg_item *debug   = NULL;
	int       succs   = 0;
	NTSTATUS  status;

	do
	{
		if (data->in_size != sizeof(cont_dat)) {
			break;
		}

		status = PsLookupProcessByProcessId(
			msg->proc_id, &process
			);

		if (NT_SUCCESS(status) == FALSE) {
			break;
		}

		if ( (debug = dbg_find_item(IoGetCurrentProcess(), process)) == NULL ) {
			break;
		}

		succs = channel_send(debug->chan, msg);
	} while (0);

	if (process != NULL) {
		ObDereferenceObject(process);
	}

	if (debug != NULL) {
		dbg_deref_item(debug);
	}

	return succs;
}

void  
load_image_notify_routine(
	PUNICODE_STRING  FullImageName,
	HANDLE  ProcessId,
	PIMAGE_INFO  ImageInfo
	)
{
	dbg_item  *	debug	= NULL;
	dbg_msg		msg;
	cont_dat	cont;
	PEPROCESS 	dbgd;
	NTSTATUS	status;
	
	DbgMsg("load_image_notify_routine 0x%p %ws \n",ProcessId, FullImageName->Buffer);
	
	do
	{
		status = PsLookupProcessByProcessId(ProcessId,&dbgd) != STATUS_SUCCESS;
		if ( NT_SUCCESS(status) == FALSE ){
			break;
		}
		
		if ( (debug = dbg_find_item(NULL, dbgd)) == NULL ) {
			break;
		}
		
		/* check event mask */
		if ( (debug->filter.event_mask & DBG_LOAD_DLL) == 0 ) {
			break;
		}
		
		/* setup debug message */
		msg.process_id             = ProcessId;
		msg.thread_id              = PsGetCurrentThreadId(); 
		msg.event_code             = DBG_LOAD_DLL;
		
		/* copy image info */
		msg.dll_load.dll_image_base = ImageInfo->ImageBase;
		msg.dll_load.dll_image_size = ImageInfo->ImageSize;
		
		memset(&msg.dll_load.dll_name, 0, MAX_PATH * sizeof(WCHAR));
		
		/* copy full image name */
		memcpy (
				&msg.dll_load.dll_name, 
				FullImageName->Buffer, 
				FullImageName->Length > MAX_PATH * sizeof(WCHAR) ? MAX_PATH * sizeof(WCHAR) : FullImageName->Length
				);
		
		/* send debug message and recive replay */
		if (channel_send_recv(debug->chan, &msg, &cont) == 0) {
			break;
		}
		
	} while (0);
	
	if (debug != NULL) {
		dbg_deref_item(debug);
	}
	
	return;
}

int init_debug(HANDLE h_key)
{
	int succs = 0;
	u32 offs, idx1;
	u32 idx2, idx3;
	u32 offs2;

	do
	{
		/* get offset of NtTerminateProcess */
		if (reg_query_val(h_key, L"sym__NtTerminateProcess@8", &offs, sizeof(offs)) == 0) {
			break;
		}

		NtTerminateProcess = addof(ntkrn_base, offs);

		/* get offset of NtNtResumeThread */
		if (reg_query_val(h_key, L"sym__NtResumeThread@8", &offs, sizeof(offs)) == 0) {
			break;
		}

		NtResumeThread = addof(ntkrn_base, offs);

		/* get index of ZwTerminateProcess */
		if (reg_query_val(h_key, L"sym_ZwTerminateProcess", &idx1, sizeof(idx1)) == 0) {
			break;
		}

		/* get index of ZwCreateThread */
		if (reg_query_val(h_key, L"sym_ZwCreateThread", &idx2, sizeof(idx2)) == 0) {
			break;
		}

		/* get index of ZwTerminateThread */
		if (reg_query_val(h_key, L"sym_ZwTerminateThread", &idx3, sizeof(idx3)) == 0) {
			break;
		}

		/* get offset of ThreadListEntry in _ETHREAD */
		if (reg_query_val(h_key, L"sym__ETHREAD.ThreadListEntry",
			&thread_enth_offs, sizeof(thread_enth_offs)) == 0) {
			break;
		}

		/* get offset of _KiDispatchException@20 */
		if (reg_query_val(h_key, L"sym__KiDispatchException@20", &offs2, sizeof(offs2)) == 0) {
			break;
		}

		set_sdt_hook(
			idx1, NewZwTerminateProcess, &OldZwTerminateProcess
			);

		set_sdt_hook(
			idx2, NewZwCreateThread, &OldZwCreateThread
			);

		set_sdt_hook(
			idx3, NewZwTerminateThread, &OldZwTerminateThread
			);

		OldKiDispatchException = hook_code(
			addof(ntkrn_base, offs2), NewKiDispatchException
			);
		
		PsSetLoadImageNotifyRoutine(&load_image_notify_routine);

		succs = 1;
	} while (0);

	return succs;
}
