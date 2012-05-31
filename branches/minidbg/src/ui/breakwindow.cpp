#include "stdafx.h"
#include "breakwindow.h"

#include "dbgapi.h"
#include "udis86.h"

const size_t max_instruction_len = 16;

void break_window::created_slot(dbg_msg msg)
{
}

void break_window::debug_slot(dbg_msg msg)
{
}

void break_window::breakpoint_slot(dbg_msg msg)
{
	int row = rowCount();
    insertRow(row);
	QTableWidgetItem *qItem = new QTableWidgetItem(tr("0x11234"));
	setItem(row, 0, qItem);
	qItem = new QTableWidgetItem(tr("1"));
	setItem(row, 1, qItem);
	qItem = new QTableWidgetItem(tr("soft"));
	setItem(row, 2, qItem);
}

void break_window::terminated_slot(dbg_msg msg)
{
	clear();
}

void break_window::start_thread_slot(dbg_msg msg)
{
	// @todo process context specific breakpoints (add)
}

void break_window::exit_thread_slot(dbg_msg msg)
{
	// @todo process context non-specific breakpoints (remove)
}

void break_window::exception_slot(dbg_msg msg)
{
	//m_tracer.getBreakpointsManager(
}

void break_window::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu(this);
	menu->addAction(tr("Add breakpoint"));
	menu->addAction(tr("Modify breakpoint"));
	menu->addAction(tr("Delete breakpoint"));
	menu->exec(event->globalPos());
	delete menu;
}
