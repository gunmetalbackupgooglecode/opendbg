#pragma once
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include "ui_main_window.h"

#include "cliwindow.h"

class main_window : public QMainWindow
{
	Q_OBJECT

public:
	main_window(QWidget *parent = 0, Qt::WFlags flags = 0);
	~main_window();

private:
	Ui::main_windowClass ui;
	cli_window *m_cli;
};

#endif // MAIN_WINDOW_H
