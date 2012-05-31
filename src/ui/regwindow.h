#pragma once
#ifndef REGWINDOW_H__
#define REGWINDOW_H__

#include <QtGui/QTextEdit>
#include "dbgapi.h"

enum registers_group {
	REGS_FPU,
	REGS_MMX,
	REGS_XMM,
	REGS_DEBUG,
	REGS_3DNOW,
	REGS_NO_GROUP
};

typedef struct {
	registers_group group_id;
	QString group_name;
} registers_groups;

static const registers_groups reg_groups_desc[] = {
	{ REGS_FPU, QString("FPU") },
	{ REGS_MMX, QString("MMX") },
	{ REGS_XMM, QString("XMM") },
	{ REGS_DEBUG, QString("Debug") },
	{ REGS_3DNOW, QString("3DNow!") }
};

class reg_window : public QTextEdit
{
	Q_OBJECT
public:
	reg_window()
	 : QTextEdit(),
	 current_registers_group(REGS_FPU)
	{
	}

	reg_window(const QString &text, QWidget *parent = 0)
	 : QTextEdit(text)
	{
	}

private:
	// FIXME: replace with abstract handlers
	registers_group current_registers_group;
	void contextMenuEvent(QContextMenuEvent *event);

public slots:
	void created_slot(dbg_msg msg);
	void debug_slot(dbg_msg msg);
	void breakpoint_slot(dbg_msg msg);
	void terminated_slot(dbg_msg msg);
	void exception_slot(dbg_msg msg);
};

#endif // REGWINDOW_H__