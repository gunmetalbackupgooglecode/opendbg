/*! \headerfile mainframe.h
* \brief This is header that contains main application classes
* \author d1mk4
*/

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

/*! \class App mainframe.h "src/ui/inc/mainframe.h"
*   \brief This is init class for start application
*/
class App : public wxApp
{
public:

	//! Main init function
	bool OnInit();
};

/*! \class MainFrame mainframe.h "src/ui/inc/mainframe.h"
*   \brief This is main MDI window that binds all frames
*/
class MainFrame : public wxMDIParentFrame
{
public:
	//! MDI window constructor with parameters
	/*!
	* @param[in] parent for creating from parent as child
	* @param[in] id for defining window id
	* @param[in] title window title
	* @param[in] pos position where this window should displayed
	* @param[in] size height and width of the window
	* @param[in] style for defining window style
	*/
	MainFrame(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);

	//! MDI window destructor
	~MainFrame();

	//! Start debug session and load driver
	int InitDebugger();

	//! Init styles and icons for toolbar
	void InitToolBar(wxToolBar* dbgToolbar, wxToolBar* winToolBar);

	//! Startup all important windows
	void ShowUsableWindows();

	//! Handler for process all size events
	void OnSize(wxSizeEvent& event);

	//! Handler for show about dialog
	void OnAbout(wxCommandEvent& event);

	//! Handler for process quit events
	void OnQuit(wxCommandEvent& event);

	//! Handler for process window terminate events
	void OnClose(wxCloseEvent& event);

	//! Event handler for hide/show CPUFrame
	void OnCPUFrame(wxCommandEvent& event);

	//! Event handler for hide/show LogFrame
	void OnLogFrame(wxCommandEvent& event);

	//! Event handler for hide/show RegFrame
	void OnRegFrame(wxCommandEvent& event);

	//! Event handler for hide/show BPFrame
	void OnBPFrame(wxCommandEvent& event);

	//! Event handler for hide/show ModFrame
	void OnModFrame(wxCommandEvent& event);

	//! Event handler for hide/show ThreadFrame
	void OnThreadFrame(wxCommandEvent& event);

	//! Event handler for hide/show MemFrame
	void OnMemFrame(wxCommandEvent& event);

	//! Event handler for hide/show StackFrame
	void OnStackFrame(wxCommandEvent& event);

	//! Event handler for open module to debug command
	void OnOpenModule(wxCommandEvent& event);

	//! Event handler for step into command
	void OnStepInto(wxCommandEvent& event);

	//! Event handler for step over command
	void OnStepOver(wxCommandEvent& event);

	//! Event handler for until return command
	void OnTilRet(wxCommandEvent& event);

	//! Event handler for run command
	void OnRun(wxCommandEvent& event);

	//! Event handler for alert on break-poing events
	void OnMsgBP(GuiProcEvent& event);

	//! Event handler for process exception events from debugger engine
	void OnExceptHandle(TrcExceptEvent &event);

	//! send exception events from another (like debug thread) thread
	static ULONG __stdcall thunk_except_event(PTRC_EXCEPTION_EVENT evt, void *arg);

	//! send debug event from another thread (like debug thread) through exception event
	static ULONG __stdcall thunk_dbg_event(PTRC_EXCEPTION_EVENT evt, void *arg);

	//! send process event from another thread (like debug thread)
	static ULONG __stdcall thunk_proc_event(PTRC_PROCESS_EVENT evt, void *arg);

public:
	//! save current window layout to config file
	void SaveLayout();

	//! load current window layout from config file
	void LoadLayout();

private:
	boost::shared_ptr<CPUFrame> m_cpuframe; //! CPUFrame member
	boost::shared_ptr<LogFrame> m_logframe; //! LogFrame member
	boost::shared_ptr<RegFrame> m_regframe; //! RegFrame member
	boost::shared_ptr<BPFrame> m_bpframe; //! BPFrame member
	boost::shared_ptr<ModFrame> m_modframe; //! ModFrame member
	boost::shared_ptr<MemFrame> m_memframe; //! MemFrame member
	boost::shared_ptr<ThreadFrame> m_threadframe; //! ThreadFrame member
	boost::shared_ptr<StackFrame> m_stackframe; //! StackFrame member

private:
	wxAuiManager m_auimgr; //! this is wxAUI windows manager member

private:
	Debugger m_debugger; //! member of debug wrapper

private:
	uint32_t m_userAnswer; //! this member used for sends user answers to debug thread

private:
	TrcExceptEvent m_trc_except; //! except event member (for cooperation with tracer)
	TrcDebugEvent m_trc_debug; //! debug event member (for cooperation with tracer)
	TrcProcEvent m_trc_proc; //! process event member (for cooperation with tracer)

private:
	wxMutex m_debug_mutex; //! debug mutex for using in debug condition
	wxMutex m_except_mutex; //! exception mutex for using in exception condition
	wxMutex m_proc_mutex; //! process mutex for using in process condition
	wxCondition m_debug_condition; //! debug condition member for sync threads
	wxCondition m_except_condition; //! exception condition member for sync threads
	wxCondition m_proc_condition; //! process condition member for sync threads

private:
	DECLARE_EVENT_TABLE()
};


#endif // __MAINFRAME_H__
