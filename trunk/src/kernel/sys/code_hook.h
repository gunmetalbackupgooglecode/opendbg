#ifndef _CODE_HOOK_
#define _CODE_HOOK_

#define HOTPATCH_NOT_AVAILABLE 0
#define HOTPATCH_AVAILABLE 1
#define HOTPATCH_ALREADY_SET 2

int set_hook(void *src_proc, void *new_proc, void **old_proc);
int	hook_code(void *src_proc, void *new_proc, void **old_proc);
int	hot_patch(void *src_proc, void *new_proc, void **old_proc);

#endif
