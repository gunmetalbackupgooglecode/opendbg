#include "stdafx.h"
#include "mainwindow.h"

main_window::main_window(QWidget *parent, Qt::WFlags flags)
 : QMainWindow(parent, flags),
   m_tracer()
{
	ui.setupUi(this);
	setWindowTitle(tr("opendbg"));
	QDockWidget* cli_dock    = new QDockWidget(tr("Command Line"), this);
	QDockWidget* cpu_dock = new QDockWidget(tr("CPU View"), this);
	QDockWidget* log_dock = new QDockWidget(tr("Log"), this);

	m_cpu_window = new cpu_window();
	m_cli_window = new cli_window("> ");
	m_log_window = new log_window();

	cpu_dock->setWidget(m_cpu_window);
	cli_dock->setWidget(m_cli_window);
	log_dock->setWidget(m_log_window);
	this->addDockWidget(Qt::LeftDockWidgetArea, cpu_dock);
	this->addDockWidget(Qt::TopDockWidgetArea, cli_dock);
	this->addDockWidget(Qt::BottomDockWidgetArea, log_dock);

	init_menu();

	connect(m_log_window, SIGNAL(log_window::created_handler()), )

	m_tracer.add_created_slot(boost::bind(&log_window::created_handler, m_log_window, _1));
	m_tracer.add_trace_slot(boost::bind(&log_window::debug_handler, m_log_window, _1));
	m_tracer.add_breakpoint_slot(boost::bind(&log_window::breakpoint_handler, m_log_window, _1));
	//m_tracer.add_terminated_slot(boost::bind(&debugger_cli::terminated_slot, this, _1));
	//m_tracer.add_start_thread_slot(boost::bind(&debugger_cli::start_thread_slot, this, _1));
	//m_tracer.add_exit_thread_slot(boost::bind(&debugger_cli::exit_thread_slot, this, _1));
	//m_tracer.add_exception_slot(boost::bind(&debugger_cli::exception_slot, this, _1));
	//m_tracer.add_dll_load_slot(boost::bind(&debugger_cli::dll_load_slot, this, _1));
}

main_window::~main_window()
{
	delete m_cpu_window;
	delete m_cli_window;
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

	m_debug_menu->addAction(m_step_into_action);
	m_debug_menu->addAction(m_step_over_action);
	m_debug_menu->addAction(m_step_out_action);
}

void main_window::open()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Executables (*.exe *.dll)"));
	m_tracer.set_image_name(filename.toStdString());
	boost::thread dbg_thread(boost::ref(m_tracer));
}

void main_window::exit()
{
	this->~main_window();
}

void main_window::step_into()
{
}

void main_window::step_over()
{

}

void main_window::step_out()
{

}