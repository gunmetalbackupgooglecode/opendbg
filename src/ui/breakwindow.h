#pragma once
#ifndef BREAKWINDOW_H__
#define BREAKWINDOW_H__

#include <QtGui/QTableWidget>
#include <QHeaderView>
#include "dbgapi.h"

class break_window : public QTableWidget
{
	Q_OBJECT
public:
	break_window(QWidget *parent = 0)
	 : QTableWidget(0, 4, parent)
	{
		QStringList header;
		header.push_back(tr("address"));
		header.push_back(tr("size"));
		header.push_back(tr("state"));
		header.push_back(tr("original"));
		setHorizontalHeaderLabels(header);
		horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
		verticalHeader()->hide();
		setShowGrid(false);
		setSelectionMode(QAbstractItemView::SingleSelection);
		setSelectionBehavior(QAbstractItemView::SelectRows);
	}

private:
	void contextMenuEvent(QContextMenuEvent *event);

public slots:
	void created_slot(dbg_msg msg);
	void debug_slot(dbg_msg msg);
	void breakpoint_slot(dbg_msg msg);
	void terminated_slot(dbg_msg msg);
	void start_thread_slot(dbg_msg msg);
	void exit_thread_slot(dbg_msg msg);
	void exception_slot(dbg_msg msg);
};

#endif // BREAKWINDOW_H__