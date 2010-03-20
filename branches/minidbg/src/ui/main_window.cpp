#include "stdafx.h"
#include "main_window.h"

main_window::main_window(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	setWindowTitle(tr("opendbg"));
	QDockWidget *dock = new QDockWidget(tr("Command Line"), this);
	
	m_cli = new cli_window("> ");
	dock->setWidget(m_cli);
	this->addDockWidget(Qt::TopDockWidgetArea, dock);
	this->statusBar()->showMessage(tr("hello"));
}

main_window::~main_window()
{
	delete m_cli;
}
