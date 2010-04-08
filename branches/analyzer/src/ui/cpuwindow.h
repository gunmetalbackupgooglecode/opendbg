#pragma once
#ifndef DISASMFRAME_H__
#define DISASMFRAME_H__

#include <QtGui/QTextEdit>

class cpu_window : public QTextEdit
{
	Q_OBJECT
public:
	cpu_window::cpu_window()
	 : QTextEdit()
	{
	}

	cpu_window::cpu_window(const QString &text, QWidget *parent /*= 0*/)
	 : QTextEdit(text, parent)
	{
	}
};

#endif // DISASMFRAME_H__