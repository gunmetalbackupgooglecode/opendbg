#ifndef _SYSCALL_
#define _SYSCALL_

typedef struct _syscall {
	int   number;   /* syscall number */
	void *in_data;  /* input buffer   */
	u32   in_size;  /* input size     */
	void *out_data; /* output buffer  */
	u32   out_size; /* output size    */
} syscall;

typedef int (*scproc)(syscall *data);

int init_syscall(HANDLE h_key);

#endif