/*! \file logframe.h
* \brief window frame for showing logs
* \author d1mk4
*/

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

/*! \class LogFrame logframe.h "src/ui/inc/logframe.h"
*   \brief This frame is log window for display logs
*/
class LogFrame : public ChildFrame
{
public:
	/*! Constructor
	* @param[in] parent parent MDI window handler
	* @param[in] title string title
	*/
	LogFrame(wxMDIParentFrame *parent, const wxString& title);

	//! Destructor
	~LogFrame();

	//! this function sends messages from dbg_event to Log
	void DbgEventToLog(TrcDebugEvent &event);

	//! this function sends messages from except_event to Log
	void ExceptEventToLog(TrcExceptEvent &event);

	//! this function sends функция отправки сообщений от proc_event в Log
	void ProcEventToLog(TrcProcEvent &event);

	//! this is a handler for process resizing events to this window
	void OnSize(wxSizeEvent& event);

	//! output message to Log window
	/*!
	* @param[in] str message for output to Log frame
	*/
	void Print(const wxString &str);

private:
	//! save frame parameters
	virtual void SaveParams();

	//! load frame parameters
	virtual void LoadParams();

private:
	wxTextCtrl m_textCtrl; //! text control member
	size_t m_counter; //! message counter

private:
	DECLARE_EVENT_TABLE()
};

#endif // __LOGFRAME_H__