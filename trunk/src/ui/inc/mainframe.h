#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "cpuframe.h"
#include "logframe.h"
#include "regframe.h"
#include "bpframe.h"
#include "modframe.h"
#include "threadframe.h"
#include "memframe.h"
#include "stackframe.h"

//#include "bitmaps/new.xpm"
//#include "bitmaps/open.xpm"
//#include "bitmaps/save.xpm"
//#include "bitmaps/copy.xpm"
//#include "bitmaps/cut.xpm"
//#include "bitmaps/paste.xpm"
//#include "bitmaps/print.xpm"
//#include "bitmaps/help.xpm"

class App : public wxApp
{
public:
	bool OnInit();
};

// Define a new frame
class MainFrame : public wxMDIParentFrame
{
public:
	MainFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);

	~MainFrame();

	int InitDebugger();
	void InitToolBar(wxToolBar* dbgToolbar, wxToolBar* winToolBar);
	void ShowUsableWindows();

	void OnSize(wxSizeEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnClose(wxCloseEvent& event);
	
	// обработчики сообщения в главном меню, типа скрыть/показать окно
	template<typename T>
	void OnShow(wxCommandEvent& event);

	void OnCPUFrame(wxCommandEvent& event);
	void OnLogFrame(wxCommandEvent& event);
	void OnRegFrame(wxCommandEvent& event);
	void OnBPFrame(wxCommandEvent& event);
	void OnModFrame(wxCommandEvent& event);
	void OnThreadFrame(wxCommandEvent& event);
	void OnMemFrame(wxCommandEvent& event);
	void OnStackFrame(wxCommandEvent& event);

	void OnOpenModule(wxCommandEvent& event);

	void OnStepInfo(wxCommandEvent& event);
	void OnStepOver(wxCommandEvent& event);
	void OnTilRet(wxCommandEvent& event);
	void OnRun(wxCommandEvent& event);

	void OnMsgBP(GuiProcEvent& event);

	void OnExceptHandle(TrcExceptEvent &event);

	static ULONG __stdcall thunk_except_event(PTRC_EXCEPTION_EVENT evt, void *arg);
	static ULONG __stdcall thunk_dbg_event(PTRC_EXCEPTION_EVENT evt, void *arg);
	static ULONG __stdcall thunk_proc_event(PTRC_PROCESS_EVENT evt, void *arg);

public:
	void SaveLayout();
	void LoadLayout();

private:
	// окошки
	boost::shared_ptr<CPUFrame> m_cpuframe;
	boost::shared_ptr<LogFrame> m_logframe;
	boost::shared_ptr<RegFrame> m_regframe;
	boost::shared_ptr<BPFrame> m_bpframe;
	boost::shared_ptr<ModFrame> m_modframe;
	boost::shared_ptr<MemFrame> m_memframe;
	boost::shared_ptr<ThreadFrame> m_threadframe;
	boost::shared_ptr<StackFrame> m_stackframe;

private:
	wxAuiManager m_auimgr;

private:
	// обертка над отладчиком
	Debugger m_debugger;

private:
	// используется для передачи ответов пользователя отладочному потоку
	ULONG m_userAnswer;

private:
	// отладочные события
	TrcExceptEvent m_trc_except;
	TrcDebugEvent m_trc_debug;
	TrcProcEvent m_trc_proc;

private:
	wxMutex m_debug_mutex;
	wxMutex m_except_mutex;
	wxMutex m_proc_mutex;
	wxCondition m_debug_condition;
	wxCondition m_except_condition;
	wxCondition m_proc_condition;

private:
	DECLARE_EVENT_TABLE()
};


#endif // __MAINFRAME_H__
