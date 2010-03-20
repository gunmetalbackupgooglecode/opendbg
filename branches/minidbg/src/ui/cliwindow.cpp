#include "stdafx.h"
#include "cliwindow.h"

cli_window::cli_window( const QString &text, QWidget *parent /*= 0*/ )
{
	QTextEdit(text, parent);
}