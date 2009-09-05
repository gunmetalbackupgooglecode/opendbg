/*
    *
	* Copyright (c) 2009
	* vol4ok <vol4ok@highsecure.ru> PGP key ID - 0x7A1C8BB4A0F34B67
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
#include "dbg_item.h"
#include "channel.h"

static LIST_ENTRY dbg_item_list;
static KSPIN_LOCK dbg_item_lock;

dbg_item *dbg_new_item(void *debugged)
{
	dbg_item *item;

	if (item = mem_alloc(sizeof(dbg_item)))
	{
		zeromem(item, sizeof(dbg_item));

		item->active    = 0;
		item->debugger  = IoGetCurrentProcess();
		item->debugged  = debugged;
		item->ref_count = 2;

		ObReferenceObject(item->debugger);
		ObReferenceObject(item->debugged);

		ExInterlockedInsertTailList(
			&dbg_item_list, &item->entry_list, &dbg_item_lock
			);

	}

	return item;
}

dbg_item *dbg_find_item(void *debgr, void *dbgd)
{
	PLIST_ENTRY entry;
	dbg_item   *item;
	dbg_item   *find = NULL;
	KIRQL       old_irql;

	KeAcquireSpinLock(
		&dbg_item_lock, &old_irql
		);

	entry = dbg_item_list.Flink;

	while (entry != &dbg_item_list)
	{
		item  = CONTAINING_RECORD(entry, dbg_item, entry_list);
		entry = entry->Flink;

		if ( (debgr == NULL || item->debugger == debgr) &&
			 (dbgd  == NULL || item->debugged == dbgd) &&
			 (item->active != 0) )
		{
			lock_inc(&item->ref_count);
			find = item;
			break;
		}
	}


	KeReleaseSpinLock(
		&dbg_item_lock, old_irql
		);

	return find;
}

void dbg_deref_item(dbg_item *item)
{
	if (lock_dec(&item->ref_count) == 0)
	{
		RemoveEntryList(
			&item->entry_list
			);

		DbgMsg("dbg_deref_item free\n");

		channel_free(item->chan);

		ObDereferenceObject(item->debugged);
		ObDereferenceObject(item->debugger);

		mem_free(item);
	}
}

void dbg_delete_item(dbg_item *item)
{
	item->active = 0;

	channel_release(item->chan);

	dbg_deref_item(item);
}

int init_dbg_item(HANDLE h_key)
{
	InitializeListHead(
		&dbg_item_list
		);

	KeInitializeSpinLock(
		&dbg_item_lock
		);

	return 1;
}
