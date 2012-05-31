#include "stdafx.h"
#include "mainwindow.h"

main_window::main_window(QWidget *parent, Qt::WFlags flags)
 : QMainWindow(parent, flags),
   m_tracer()
{
	ui.setupUi(this);
	setWindowTitle(tr("opendbg"));
	QDockWidget* cli_dock = new QDockWidget(tr("Command Line"), this);
	QDockWidget* cpu_dock = new QDockWidget(tr("CPU View"), this);
	QDockWidget* log_dock = new QDockWidget(tr("Log"), this);
	QDockWidget* reg_dock = new QDockWidget(tr("Registers"), this);
	QDockWidget* stack_dock = new QDockWidget(tr("Stack"), this);
	QDockWidget* break_dock = new QDockWidget(tr("Breakpoints"), this);

	m_cpu_window = new cpu_window();
	m_cli_window = new cli_window("> ");
	m_log_window = new log_window();
	m_reg_window = new reg_window();
	m_stk_window = new stk_window();
	m_brk_window = new break_window();
	
	m_log_window->setReadOnly(true);
	m_reg_window->setReadOnly(true);
	m_stk_window->setReadOnly(true);

	cpu_dock->setWidget(m_cpu_window);
	cli_dock->setWidget(m_cli_window);
	log_dock->setWidget(m_log_window);
	reg_dock->setWidget(m_reg_window);
	stack_dock->setWidget(m_stk_window);
	break_dock->setWidget(m_brk_window);
	this->addDockWidget(Qt::RightDockWidgetArea, reg_dock);
	this->addDockWidget(Qt::RightDockWidgetArea, stack_dock);
	this->addDockWidget(Qt::LeftDockWidgetArea, cpu_dock);
	this->addDockWidget(Qt::BottomDockWidgetArea, cli_dock);
	this->addDockWidget(Qt::BottomDockWidgetArea, log_dock);
	this->addDockWidget(Qt::BottomDockWidgetArea, break_dock);

	init_menu();
	init_toolbar();

	qRegisterMetaType<dbg_msg>("dbg_msg");

	connect(this, SIGNAL(created(dbg_msg)), m_log_window, SLOT(created_slot(dbg_msg)));
	connect(this, SIGNAL(debuged(dbg_msg)), m_log_window, SLOT(debug_slot(dbg_msg)));
	connect(this, SIGNAL(breakpointed(dbg_msg)), m_log_window, SLOT(breakpoint_slot(dbg_msg)));
	connect(this, SIGNAL(terminated(dbg_msg)), m_log_window, SLOT(terminated_slot(dbg_msg)));
	connect(this, SIGNAL(started_thread(dbg_msg)), m_log_window, SLOT(start_thread_slot(dbg_msg)));
	connect(this, SIGNAL(exited_thread(dbg_msg)), m_log_window, SLOT(exit_thread_slot(dbg_msg)));
	connect(this, SIGNAL(exceptioned(dbg_msg)), m_log_window, SLOT(exception_slot(dbg_msg)));
	connect(this, SIGNAL(dll_loaded(dbg_msg)), m_log_window, SLOT(dll_load_slot(dbg_msg)));

	connect(this, SIGNAL(created(dbg_msg)), m_reg_window, SLOT(created_slot(dbg_msg)));
	connect(this, SIGNAL(debuged(dbg_msg)), m_reg_window, SLOT(debug_slot(dbg_msg)));
	connect(this, SIGNAL(breakpointed(dbg_msg)), m_reg_window, SLOT(breakpoint_slot(dbg_msg)));
	connect(this, SIGNAL(terminated(dbg_msg)), m_reg_window, SLOT(terminated_slot(dbg_msg)));
	connect(this, SIGNAL(exceptioned(dbg_msg)), m_reg_window, SLOT(exception_slot(dbg_msg)));
	
	connect(this, SIGNAL(created(dbg_msg)), m_stk_window, SLOT(created_slot(dbg_msg)));
	connect(this, SIGNAL(debuged(dbg_msg)), m_stk_window, SLOT(debug_slot(dbg_msg)));
	connect(this, SIGNAL(terminated(dbg_msg)), m_stk_window, SLOT(terminated_slot(dbg_msg)));

	connect(this, SIGNAL(created(dbg_msg)), m_brk_window, SLOT(created_slot(dbg_msg)));
	connect(this, SIGNAL(debuged(dbg_msg)), m_brk_window, SLOT(debug_slot(dbg_msg)));
	connect(this, SIGNAL(breakpointed(dbg_msg)), m_brk_window, SLOT(breakpoint_slot(dbg_msg)));
	connect(this, SIGNAL(terminated(dbg_msg)), m_brk_window, SLOT(terminated_slot(dbg_msg)));
	connect(this, SIGNAL(started_thread(dbg_msg)), m_brk_window, SLOT(start_thread_slot(dbg_msg)));
	connect(this, SIGNAL(exited_thread(dbg_msg)), m_log_window, SLOT(exit_thread_slot(dbg_msg)));
	connect(this, SIGNAL(exceptioned(dbg_msg)), m_brk_window, SLOT(exception_slot(dbg_msg)));

	m_tracer.add_created_slot(boost::bind(&main_window::created_handler, this, _1));
	m_tracer.add_trace_slot(boost::bind(&main_window::debug_handler, this, _1));
	m_tracer.add_breakpoint_slot(boost::bind(&main_window::breakpoint_handler, this, _1));
	m_tracer.add_terminated_slot(boost::bind(&main_window::terminated_handler, this, _1));
	m_tracer.add_start_thread_slot(boost::bind(&main_window::start_thread_handler, this, _1));
	m_tracer.add_exit_thread_slot(boost::bind(&main_window::exit_thread_handler, this, _1));
	m_tracer.add_dll_load_slot(boost::bind(&main_window::dll_load_handler, this, _1));
}

main_window::~main_window()
{
	delete m_cpu_window;
	delete m_cli_window;
	delete m_log_window;
	delete m_reg_window;
	delete m_stk_window;
}

void main_window::init_menu()
{
	m_file_menu = menuBar()->addMenu(tr("&File"));

	m_open_action = new QAction(tr("&Open"), this);
	m_open_action->setShortcut(QKeySequence::Open);
	m_open_action->setStatusTip(tr("Open module for debugging"));
	connect(m_open_action, SIGNAL(triggered()), this, SLOT(open()));

	m_exit_action = new QAction(tr("&Exit"), this);
	m_exit_action->setShortcut(QKeySequence::Close);
	m_exit_action->setStatusTip(tr("Exit from program"));
	connect(m_exit_action, SIGNAL(triggered()), this, SLOT(exit()));

	m_file_menu->addAction(m_open_action);
	m_file_menu->addAction(m_exit_action);
	
	m_debug_menu = menuBar()->addMenu(tr("&Debug"));

	m_step_into_action = new QAction(tr("Step &Into"), this);
	m_step_into_action->setShortcut(tr("F7"));
	m_step_into_action->setStatusTip(tr("Step into the code"));
	connect(m_step_into_action, SIGNAL(triggered()), this, SLOT(step_into()));

	m_step_over_action = new QAction(tr("Step Over"), this);
	m_step_over_action->setShortcut(tr("F8"));
	m_step_over_action->setStatusTip(tr("Step over the code =)"));
	connect(m_step_over_action, SIGNAL(triggered()), this, SLOT(step_over()));

	m_step_out_action = new QAction(tr("Step Out"), this);
	m_step_out_action->setShortcut(tr("F9"));
	m_step_out_action->setStatusTip(tr("Step out the code"));
	connect(m_step_out_action, SIGNAL(triggered()), this, SLOT(step_out()));

	m_run_action = new QAction(tr("Run"), this);
	m_run_action->setShortcut(tr("F5"));
	m_run_action->setStatusTip(tr("Run the program"));
	connect(m_run_action, SIGNAL(triggered()), this, SLOT(run()));

	m_options_menu = menuBar()->addMenu(tr("&Options"));

	m_options_action = new QAction(tr("Debugger o&ptions"), this);
	m_options_action->setShortcut(tr("Ctrl-F12"));
	m_options_action->setStatusTip(tr("Debugger options"));
	connect(m_run_action, SIGNAL(triggered()), this, SLOT(options()));
	
	m_options_menu->addAction(m_options_action);

	m_debug_menu->addAction(m_step_into_action);
	m_debug_menu->addAction(m_step_over_action);
	m_debug_menu->addAction(m_step_out_action);
	m_debug_menu->addAction(m_run_action);
}

void main_window::init_toolbar()
{
	ui.mainToolBar->setObjectName(tr("Toolbar"));
	m_btn_open_action = ui.mainToolBar->addAction(QIcon(":/main_window/open"), tr("Open"));
	m_btn_open_action->setStatusTip(tr("Open module for debugging"));
	connect(m_btn_open_action, SIGNAL(triggered()), this, SLOT(open()));

	m_btn_restart_action = ui.mainToolBar->addAction(QIcon(":/main_window/restart"), tr("Restart"));
	m_btn_restart_action->setStatusTip(tr("Restart module"));
	connect(m_btn_restart_action, SIGNAL(triggered()), this, SLOT(restart()));

	m_btn_stop_action = ui.mainToolBar->addAction(QIcon(":/main_window/close"), tr("Close"));
	m_btn_stop_action->setStatusTip(tr("Stop debugging"));
	connect(m_btn_stop_action, SIGNAL(triggered()), this, SLOT(stop()));

	ui.mainToolBar->addSeparator();

	m_btn_run_action = ui.mainToolBar->addAction(QIcon(":/main_window/run"), tr("Run"));
	m_btn_run_action->setStatusTip(tr("Run module"));
	connect(m_btn_run_action, SIGNAL(triggered()), this, SLOT(run()));

	m_btn_stepin_action = ui.mainToolBar->addAction(QIcon(":/main_window/stepin"), tr("Step Into"));
	m_btn_stepin_action->setStatusTip(tr("Step into"));
	connect(m_btn_stepin_action, SIGNAL(triggered()), this, SLOT(step_into()));

	m_btn_stepover_action = ui.mainToolBar->addAction(QIcon(":/main_window/stepover"), tr("Step Over"));
	m_btn_stepover_action->setStatusTip(tr("Step over"));
	connect(m_btn_stepover_action, SIGNAL(triggered()), this, SLOT(step_over()));
}

void main_window::open()
{
	filename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Executables (*.exe *.dll)"));
	if (filename.size()) {
		m_tracer.set_image_name(filename.toStdString());
		boost::thread dbg_thread(boost::ref(m_tracer));
	}
}

void main_window::exit()
{
	this->~main_window();
}

void main_window::run()
{
	m_condition.notify_all();
}

void main_window::step_into()
{
	m_tracer.step_into();
	m_condition.notify_all();
}

void main_window::step_over()
{
	m_tracer.step_over();
	m_condition.notify_all();
}

void main_window::step_out()
{
	//m_tracer.step_out();
	//m_condition.notify_all();
}

void main_window::stop()
{
	m_tracer.stop_process();
	m_condition.notify_all();
}

void main_window::restart()
{
	m_tracer.stop_process();
	m_condition.notify_all();
	if (filename.size()) {
		m_tracer.set_image_name(filename.toStdString());
		boost::thread dbg_thread(boost::ref(m_tracer));
	}
	m_condition.notify_all();
}

void main_window::options()
{
}

void main_window::created_handler(dbg_msg msg)
{
	emit created(msg);
}

void main_window::debug_handler(dbg_msg msg)
{
	emit debuged(msg);
	lock_t lock(m_mutex);
	m_condition.wait(lock);
}

void main_window::breakpoint_handler(dbg_msg msg)
{
	emit breakpointed(msg);
	lock_t lock(m_mutex);
	m_condition.wait(lock);
}

void main_window::terminated_handler(dbg_msg msg)
{
	emit terminated(msg);
}

void main_window::start_thread_handler(dbg_msg msg)
{
	emit started_thread(msg);
}

void main_window::exit_thread_handler(dbg_msg msg)
{
	emit exited_thread(msg);
}

void main_window::exception_handler(dbg_msg msg)
{
	emit exceptioned(msg);
	lock_t lock(m_mutex);
	m_condition.wait(lock);
}

void main_window::dll_load_handler(dbg_msg msg)
{
	emit dll_loaded(msg);
}
