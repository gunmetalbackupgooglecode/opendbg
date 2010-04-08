#ifndef _DBG_ITEM_
#define _DBG_ITEM_

#include "channel.h"
#include "dbg_def.h"

typedef struct _dbg_item {
	LIST_ENTRY  entry_list;
	PEPROCESS   debugger;
	PEPROCESS   debugged;
	int         active;
	u32         ref_count;
	channel    *chan;
	event_filt  filter;

} dbg_item;

int init_dbg_item(HANDLE h_key);

dbg_item *dbg_new_item(void *debugged);
dbg_item *dbg_find_item(void *debgr, void *dbgd);
void      dbg_deref_item(dbg_item *item);
void      dbg_delete_item(dbg_item *item);

#endif
