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
#include "misc.h"
#include "extdef.h"

int dbg_lock_um_buff(
		buf_lock *lock, void *buff, u32 size
		)
{
	void *data;
	int   locked;
	int   succs;

	locked = 0; succs = 0; data = NULL;
	lock->mdl = NULL; lock->data = NULL;
	do
	{
		if ( (lock->mdl = IoAllocateMdl(buff, size, FALSE, FALSE, NULL)) == NULL ) {
			break;
		}

		__try 
		{
			MmProbeAndLockPages(
				lock->mdl, UserMode, IoModifyAccess
				);

			locked = 1;
		}

		__except(EXCEPTION_EXECUTE_HANDLER) {
			break;
		}

		if ( (data = MmMapLockedPages(lock->mdl, KernelMode)) == NULL ) {
			break;
		}

		lock->data = data; succs = 1;
	} while (0);

	if (data == NULL) 
	{
		if (locked != 0) {
			MmUnlockPages(lock->mdl);
		}

		if (lock->mdl != NULL) {
			IoFreeMdl(lock->mdl);
		}
	}

	return succs;
}

void dbg_unlock_um_buff(buf_lock *lock)
{
	MmUnmapLockedPages(
		lock->data, lock->mdl
		);

	MmUnlockPages(lock->mdl);
	IoFreeMdl(lock->mdl);
}

void *get_kernel_base()
{
	PSYSTEM_MODULE_INFORMATION_EX info;
	NTSTATUS                      status;
	u32                           req_len;
	void                         *base;

	info = NULL; base = NULL;
	do
	{
		status = ZwQuerySystemInformation(
			SystemModuleInformation, NULL, 0, &req_len
			);

		if (status != STATUS_INFO_LENGTH_MISMATCH) {
			break;
		}

		if ( (info = mem_alloc(req_len)) == NULL ) {
			break;
		}

		status = ZwQuerySystemInformation(
			SystemModuleInformation, info, req_len, NULL
			);

		if (NT_SUCCESS(status) == FALSE) {
			break;
		}

		base = info->Modules[0].Base;
	} while (0);

	if (info != NULL) {
		mem_free(info);
	}

	return base;
}

int start_system_thread(
		IN PKSTART_ROUTINE thread_start,
		IN PVOID           context
		)
{
	OBJECT_ATTRIBUTES obj;
	HANDLE            h_thread;
	NTSTATUS          status;
	int               succs;

	InitializeObjectAttributes(
		&obj, NULL,
		OBJ_KERNEL_HANDLE,
		NULL, NULL
		);

	status = PsCreateSystemThread(
		          &h_thread,
				  THREAD_ALL_ACCESS,
				  &obj, NULL, NULL, 
				  thread_start, context
				  );

	if (NT_SUCCESS(status)) 
	{
		succs = 1;
		ZwClose(h_thread);
	}

	return succs;
}

HANDLE open_process_by_ptr(
		   PEPROCESS process, u32 attrib
		   )
{
	HANDLE h_proc;

	ObOpenObjectByPointer(
		process, attrib, NULL, PROCESS_ALL_ACCESS,
	   *PsProcessType, KernelMode, &h_proc
	   );

	return h_proc;
}

u64 mem_open()
{
	u64 cr0;

	_disable();
	cr0 = __readcr0();

	__writecr0(cr0 & 0xFFFEFFFF);

	return cr0;
}

void mem_close(u64 cr0)
{
	__writecr0(cr0);
	_enable();
}