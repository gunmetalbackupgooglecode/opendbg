#pragma once
#ifndef LOGWINDOW_H__
#define LOGWINDOW_H__

#include <QtGui/QTextEdit>
#include "dbg_def.h"

class log_window : public QTextEdit
{
	Q_OBJECT
public:
	log_window()
	 : QTextEdit()
	{
	}

	log_window(const QString &text, QWidget *parent = 0)
	 : QTextEdit(text)
	{
	}

public slots:
	void created_slot(dbg_msg msg);
	void debug_slot(dbg_msg msg);
	void breakpoint_slot(dbg_msg msg);
	void terminated_slot(dbg_msg msg);
	void start_thread_slot(dbg_msg msg);
	void exit_thread_slot(dbg_msg msg);
	void exception_slot(dbg_msg msg);
	void dll_load_slot(dbg_msg msg);
};

#endif // LOGWINDOW_H__