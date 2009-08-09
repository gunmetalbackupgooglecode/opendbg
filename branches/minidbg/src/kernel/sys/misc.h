#ifndef _MISC_
#define _MISC_

typedef struct _buf_lock {
	PMDL mdl;
	void *data;
} buf_lock;

int dbg_lock_um_buff(
		buf_lock *lock, void *buff, u32 size
		);

void dbg_unlock_um_buff(buf_lock *lock);

void *get_kernel_base();

int start_system_thread(
		IN PKSTART_ROUTINE thread_start,
		IN PVOID           context
		);

HANDLE open_process_by_ptr(
		   PEPROCESS process, u32 attrib
		   );

u64  mem_open();
void mem_close(u64 cr0);

#endif