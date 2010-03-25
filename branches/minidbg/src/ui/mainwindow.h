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

typedef boost::mutex              mutex_t;
typedef boost::mutex::scoped_lock lock_t;
typedef boost::condition_variable condition_t;

public:
	main_window(QWidget *parent = 0, Qt::WFlags flags = 0);
	~main_window();

protected:
	void context_menu_event(QContextMenuEvent* event);

signals:
	void created(dbg_msg msg);
	void debuged(dbg_msg msg);
	void breakpointed(dbg_msg msg);
	void terminated(dbg_msg msg);
	void started_thread(dbg_msg msg);
	void exited_thread(dbg_msg msg);
	void exceptioned(dbg_msg msg);
	void dll_loaded(dbg_msg msg);

private slots:
	void open();
	void exit();

	void step_into();
	void step_over();
	void step_out();
	void run();

private:
	void init_menu();

	void created_handler(dbg_msg msg);
	void debug_handler(dbg_msg msg);
	void breakpoint_handler(dbg_msg msg);
	void terminated_handler(dbg_msg msg);
	void start_thread_handler(dbg_msg msg);
	void exit_thread_handler(dbg_msg msg);
	void exception_handler(dbg_msg msg);
	void dll_load_handler(dbg_msg msg);

private:
	QMenu* m_file_menu;
	QMenu* m_debug_menu;

private:
	QAction* m_open_action;
	QAction* m_exit_action;
	QAction* m_step_into_action;
	QAction* m_step_over_action;
	QAction* m_step_out_action;
	QAction* m_run_action;

private:
	Ui::main_windowClass ui;
	cli_window* m_cli_window;
	cpu_window* m_cpu_window;
	log_window* m_log_window;

private:
	trc::tracer m_tracer;
	condition_t m_condition;
	mutex_t     m_mutex;

};

#endif // MAINWINDOW_H__
