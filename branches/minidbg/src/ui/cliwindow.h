#pragma once
#ifndef CLIWINDOW_H__
#define CLIWINDOW_H__

#include <QtGui/QTextEdit>

class cli_window : public QTextEdit
{
	Q_OBJECT
public:
	cli_window(const QString &text, QWidget *parent = 0)
	 : QTextEdit(text)
	{
	}
};

#endif // CLIWINDOW_H__