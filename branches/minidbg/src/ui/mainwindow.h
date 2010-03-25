#pragma once
#ifndef MAINWINDOW_H__
#define MAINWINDOW_H__

#include <QtGui/QMainWindow>
#include <QtGui/QTextEdit>
#include "ui_main_window.h"

#include "cpuwindow.h"
#include "cliwindow.h"
#include "logwindow.h"

#include "tracer.h"

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class main_window : public QMainWindow
{
	Q_OBJECT

public:
	main_window(QWidget *parent = 0, Qt::WFlags flags = 0);
	~main_window();

protected:
	void context_menu_event(QContextMenuEvent* event);

private slots:
	void open();
	void exit();

	void step_into();
	void step_over();
	void step_out();

private:
	void init_menu();
	void created_handler(dbg_msg msg);

private:
	QMenu* m_file_menu;
	QMenu* m_debug_menu;

private:
	QAction* m_open_action;
	QAction* m_exit_action;
	QAction* m_step_into_action;
	QAction* m_step_over_action;
	QAction* m_step_out_action;

private:
	Ui::main_windowClass ui;
	cli_window* m_cli_window;
	cpu_window* m_cpu_window;
	log_window* m_log_window;

private:
	trc::tracer m_tracer;
};

#endif // MAINWINDOW_H__
