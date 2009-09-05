#ifndef _CODE_HOOK_
#define _CODE_HOOK_

#define HOTPATCH_NOT_AVAILABLE 0
#define HOTPATCH_AVAILABLE 1
#define HOTPATCH_ALREADY_SET 2

int init_hook_list();
int set_hook(void *src_proc, void *new_proc, void **old_proc);

#define HOOK_TYPE_SPLICING			0x00000001
#define HOOK_TYPE_HOT_PATCH			0x00000002
#define HOOK_TYPE_DOUBLE_HOT_PATCH	0x00000003

typedef struct _hook_descriptor {
	LIST_ENTRY	list;
	u32			hook_type;
	void		*src_proc;
	void		*old_proc;
	void		*new_proc;
	u32			old_code_size;
} hook_descriptor;

#endif
