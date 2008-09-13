#ifndef __LOGFRAME_H__
#define __LOGFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "events.h"
#include "childframe.h"
#include "listview.h"
#include "debugger.h"

//////////////////////////////////////////////////////////////////////////
// класс окна Log
class LogFrame : public ChildFrame
{
public:
	// конструктор окна
	LogFrame(wxMDIParentFrame *parent, const wxString& title);

	// деструктор
	~LogFrame();

	// функция отправки сообщений от dbg_event в Log
	void DbgEventToLog(TrcDebugEvent &event);

	// функция отправки сообщений от except_event в Log
	void ExceptEventToLog(TrcExceptEvent &event);

	// функция отправки сообщений от proc_event в Log
	void ProcEventToLog(TrcProcEvent &event);

	void OnSize(wxSizeEvent& event);

	// функция вывода сообщений в Log
	void Print(const wxString &str);

private:
	virtual void SaveParams();
	virtual void LoadParams();

private:
	wxTextCtrl m_textCtrl;
	size_t m_counter;

private:
	DECLARE_EVENT_TABLE()
};

#endif // __LOGFRAME_H__