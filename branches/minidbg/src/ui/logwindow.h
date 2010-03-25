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

public:
	void created_handler(dbg_msg);
	void debug_handler(dbg_msg);
	void breakpoint_handler(dbg_msg);
	//void terminated_handler(dbg_msg);
	//void start_thread_handler(dbg_msg);
	//void exit_thread_handler(dbg_msg);
	//void exception_handler(dbg_msg);
	//void dll_load_handler(dbg_msg);
};

#endif // LOGWINDOW_H__