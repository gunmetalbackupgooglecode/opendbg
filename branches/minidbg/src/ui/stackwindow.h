#pragma once
#ifndef STACKWINDOW_H__
#define STACKWINDOW_H__

#include <QtGui/QTextEdit>
#include "dbgapi.h"

class stk_window : public QTextEdit
{
	Q_OBJECT
public:
	stk_window()
	 : QTextEdit()
	{
	}

	stk_window(const QString &text, QWidget *parent = 0)
	 : QTextEdit(text)
	{
	}

private:
	void contextMenuEvent(QContextMenuEvent *event);

public slots:
	void created_slot(dbg_msg msg);
	void debug_slot(dbg_msg msg);
	void breakpoint_slot(dbg_msg msg);
	void terminated_slot(dbg_msg msg);
	void exception_slot(dbg_msg msg);
};

#endif // STACKWINDOW_H__