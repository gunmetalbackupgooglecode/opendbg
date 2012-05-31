#include "stdafx.h"
#include "regwindow.h"

#include "dbgapi.h"

CONTEXT old;
CONTEXT ctx_zero = {0};
QString print_register_line(QString register_name, quint32 value, bool isNewValue) {
	if (!isNewValue)
		return QString("%1 %2").arg(register_name).arg(value, 8, 16, QChar('0'));
	return QString("<font color=\"red\">%1 %2</font>")
		.arg(register_name)
		.arg(value, 8, 16, QChar('0'));
}
QString preapre_registers(CONTEXT *ctx) {
	QString registers;

	// FIXME: temprorary code
	// @todo abstract classes for registers
	registers += print_register_line("EAX", ctx->Eax, ctx->Eax != old.Eax) + "<br>";
	registers += print_register_line("ECX", ctx->Ecx, ctx->Ecx != old.Ecx) + "<br>";
	registers += print_register_line("EDX", ctx->Edx, ctx->Edx != old.Edx) + "<br>";
	registers += print_register_line("EBX", ctx->Ebx, ctx->Ebx != old.Ebx) + "<br>";
	registers += print_register_line("ESP", ctx->Esp, ctx->Esp != old.Esp) + "<br>";
	registers += print_register_line("EBP", ctx->Ebp, ctx->Ebp != old.Ebp) + "<br>";
	registers += print_register_line("EDI", ctx->Edi, ctx->Edi != old.Edi) + "<br>";
	registers += print_register_line("ESI", ctx->Esi, ctx->Esi != old.Esi) + "<br>";
	registers += "<br>" + print_register_line("EIP", ctx->Eip, ctx->Eip != old.Eip);
	// EFLAGS
	// @todo show bits of EFLAGS
	registers += "<br><br>" + print_register_line("EFLAGS", ctx->EFlags, ctx->EFlags != old.EFlags);
	return registers;
}

void reg_window::created_slot(dbg_msg msg)
{
	clear();
	std::copy(&ctx_zero, &ctx_zero + 1, &old);
}

void reg_window::debug_slot(dbg_msg msg)
{
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_FULL;
	dbg_get_context(msg.thread_id, &ctx);

	setHtml(preapre_registers(&ctx));
	
	std::copy(&ctx, &ctx + 1, &old);
}

void reg_window::breakpoint_slot(dbg_msg msg)
{
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_FULL;
	dbg_get_context(msg.thread_id, &ctx);

	setHtml(preapre_registers(&ctx));
	
	std::copy(&ctx, &ctx + 1, &old);
}

void reg_window::terminated_slot(dbg_msg msg)
{
	clear();
	std::copy(&ctx_zero, &ctx_zero + 1, &old);
}

void reg_window::exception_slot(dbg_msg msg)
{
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_FULL;
	dbg_get_context(msg.thread_id, &ctx);
	
	setHtml(preapre_registers(&ctx));

	std::copy(&ctx, &ctx + 1, &old);
}

void reg_window::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu(this);
	menu->addAction(tr("Copy all registers to clipboard"));
	/// @todo add modify actions if any register selected
	for (size_t i = REGS_FPU; i < REGS_NO_GROUP; ++i) {
		if (current_registers_group != i) {
			menu->addAction(QString(tr("View %1 registers"))
				.arg(reg_groups_desc[i].group_name)
				);
		}
	}
	menu->exec(event->globalPos());
	delete menu;
}