#include "stdafx.h"
#include "stackwindow.h"

#include "dbgapi.h"


void stk_window::created_slot(dbg_msg msg)
{
	clear();
}

void stk_window::debug_slot(dbg_msg msg)
{
	CONTEXT ctx;
	u_long readed;
	ctx.ContextFlags = CONTEXT_FULL;
	dbg_get_context(msg.thread_id, &ctx);
	int stack_address = ctx.Esp;
	unsigned int t;
	QVector<unsigned int> values;
	for (int i = 0; i < 10; i++) {
		dbg_read_memory(msg.process_id, reinterpret_cast<void *>(stack_address + i * 4),
			&t, sizeof(DWORD), &readed);
		values.push_back(t);
	}
	QString stack;
	while (!values.isEmpty()) {
		t = values.back();
		values.pop_back();
		stack.append(QString("%1<br>").arg(t, 8, 16, QChar('0')));
	}
	setHtml(stack);
}

void stk_window::breakpoint_slot(dbg_msg msg)
{
}

void stk_window::terminated_slot(dbg_msg msg)
{
	clear();
}

void stk_window::exception_slot(dbg_msg msg)
{
}

void stk_window::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu(this);
	menu->addAction(tr("Show in disassembler"));
	menu->exec(event->globalPos());
	delete menu;
}