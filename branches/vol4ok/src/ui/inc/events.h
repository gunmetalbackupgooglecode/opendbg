/*! \file events.h
* \brief Debugger's events
* \author d1mk4
*/

#ifndef __EVENTS_H__
#define __EVENTS_H__

#if _MSC_VER > 1000 
    #pragma once  
#endif // _MSC_VER > 1000

#include "precomp.h"
#include "debugger.h"

/*! \class TrcExceptEvent events.h "src/ui/inc/events.h"
*   \brief This is exeption event for process it in UI
*/
class TrcExceptEvent : public wxNotifyEvent
{
public:
	//! Default constructor
	TrcExceptEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id)
	{
	}

	//! Constructor via sesId
	TrcExceptEvent(TRC_EXCEPTION_EVENT except_evt, ULONG sesId,
	               wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id),
	   m_except_event(except_evt),
	   m_sesId(sesId)
	{
	}

	//! Copy constructor
	/*! should be used to pass event with additional data among diffrent threads */
	TrcExceptEvent(const TrcExceptEvent& event)
	 : wxNotifyEvent(event)
	{
		this->m_except_event = event.m_except_event;
		this->m_sesId = event.m_sesId;
	}

	//! Clone function
	virtual wxEvent *Clone() const
	{
		return new TrcExceptEvent(*this);
	};

	//! Get except event
	TRC_EXCEPTION_EVENT getExceptEvent();

	//! Get current debug session id
	ULONG getSesId();

	//! Set except event
	void setExceptEvent(const TRC_EXCEPTION_EVENT& except_event);

	//! Set current debug session id
	void setSesId(const ULONG &sesId);

private:
	TRC_EXCEPTION_EVENT m_except_event; //! debug event TRC_EXCEPTION_EVENT
		ULONG m_sesId; //! debugger session ID 

private:
	DECLARE_DYNAMIC_CLASS(TrcExceptEvent)
};

/*! \class TrcDebugEvent events.h "src/ui/inc/events.h"
*   \brief This is debug event for process it in UI
*/
class TrcDebugEvent : public wxNotifyEvent
{
public:
	//! Default constructor
	TrcDebugEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
  : wxNotifyEvent(commandType, id)
	{
	}

	//! Constructor via sesId
	TrcDebugEvent(TRC_EXCEPTION_EVENT debug_evt, ULONG sesId,
	                  wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id),
	   m_debug_event(debug_evt),
	   m_sesId(sesId)
	{
	}

	//! Copy constructor
	/*! should be used to pass event with additional data among diffrent threads */
	TrcDebugEvent(const TrcDebugEvent& event)
	 : wxNotifyEvent(event)
	{
		this->m_debug_event = event.m_debug_event;
		this->m_sesId = event.m_sesId;
	}

	//! Clone function
	virtual wxEvent *Clone() const
	{
		return new TrcDebugEvent(*this);
	};

	//! Get except event
	TRC_EXCEPTION_EVENT getDebugEvent();

	//! Get current debug session id
	ULONG getSesId();

	//! Set except event
	void setDebugEvent(const TRC_EXCEPTION_EVENT& debug_event);

	//! Set current debug session id
	void setSesId(const ULONG& sesId);

private:
	TRC_EXCEPTION_EVENT m_debug_event; //! debug event TRC_EXCEPTION_EVENT
	ULONG m_sesId; //! debugger session ID

private:
	DECLARE_DYNAMIC_CLASS(TrcDebugEvent);
};

/*! \class TrcProcEvent events.h "src/ui/inc/events.h"
*   \brief This is debug event for process it in UI
*/
class TrcProcEvent : public wxNotifyEvent
{
public:
	//! Default constructor
	TrcProcEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id)
	{
	}

	//! Copy constructor
	/*! should be used to pass event with additional data among diffrent threads */
	TrcProcEvent(const TrcProcEvent& event)
	 : wxNotifyEvent(event)
	{
		this->m_proc_event = event.m_proc_event;
		this->m_sesId = event.m_sesId;
	}

	//! Constructor via sesId
	TrcProcEvent(TRC_PROCESS_EVENT proc_evt, ULONG sesId,
	                  wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id),
	   m_proc_event(proc_evt),
	   m_sesId(sesId)
	{
	}

	//! Clone function
	virtual wxEvent *Clone() const
	{
		return new TrcProcEvent(*this);
	};

	//! Get except event
	TRC_PROCESS_EVENT getProcEvent();

	//! Get current debug session id
	ULONG getSesId();

	//! Set except event
	void setProcEvent(const TRC_PROCESS_EVENT &proc_event);

	//! Set current debug session id
	void setSesId(const ULONG& sesId);

private:
	TRC_PROCESS_EVENT m_proc_event; //! debug event TRC_PROCESS_EVENT
	ULONG m_sesId; //! debugger session ID

private:
	DECLARE_DYNAMIC_CLASS(TrcProcEvent);
};

/*! \class GuiProcEvent events.h "src/ui/inc/events.h"
*   \brief This is proc event at the UI side for process it in UI
*/
class GuiProcEvent : public wxNotifyEvent
{
public:
	//! Default constructor
	GuiProcEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id)
	{
	}

	//! Copy constructor
	/*! should be used to pass event with additional data among diffrent threads */
	GuiProcEvent(const GuiProcEvent& event)
	 : wxNotifyEvent(event)
	{
		this->m_addrBP = event.m_addrBP;
		this->m_disasmBP = event.m_disasmBP;
		this->m_debug_event = event.m_debug_event;
		this->m_sesId = event.m_sesId;
	}

	//! set break-point address
	void setAddrBP(const wxString &addrBP);

	//! set disasm code for break point
	void setDisasmBP(const wxString &disasmBP);

	//! set current debug event
	void setDebugEvent(const TRC_EXCEPTION_EVENT &debug_event);

	//! set current sesstion id
	void setSesId(const ULONG& sesId);

	//! get break-point address
	wxString getAddrBP();

	//! get disasm code where situated this break-point
	wxString getDisasmBP();

	//! get current debug event
	TRC_EXCEPTION_EVENT getDebugEvent();

	//! get current debug session id
	ULONG getSesId(); 

	//! Clone function
	virtual wxEvent *Clone() const
	{
		return new GuiProcEvent(*this);
	};

private:
	wxString m_addrBP; //! this member used for storing bp address
	wxString m_disasmBP; //! this member used for storing disasm data
	TRC_EXCEPTION_EVENT m_debug_event; //! debug event TRC_EXCEPTION_EVENT
	ULONG m_sesId; //! debugger session ID

private:
	DECLARE_DYNAMIC_CLASS(GuiProcEvent);
};

//! callback TrcExceptEvent class
typedef void (wxEvtHandler::*TrcExceptEventFunction)(TrcExceptEvent&);

//! callback for TrcDebugEvent class
typedef void (wxEvtHandler::*TrcDebugEventFunction)(TrcDebugEvent&);

//! callback for TrcProcEvent class
typedef void (wxEvtHandler::*TrcProcEventFunction)(TrcProcEvent&);

//! callback for GuiProcEvent class
typedef void (wxEvtHandler::*GuiProcEventFunction)(GuiProcEvent&);

//! define event-map with ids
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_TRC_EXCEPT, 7776)
	DECLARE_EVENT_TYPE(wxEVT_TRC_DEBUG, 7777)
	DECLARE_EVENT_TYPE(wxEVT_TRC_PROCESS, 7778)

	DECLARE_EVENT_TYPE(wxEVT_GUI_TEXT2WINDOW, 8888)
END_DECLARE_EVENT_TYPES()

//! this enum has command ids in CPUFrame context menu
enum
{
	GUI_MSG_ADD_BP = 10001,
	GUI_MSG_DEL_BP
};

//! trc_except macros for using in event-map
#define EVT_TRC_EXCEPT(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
		wxEVT_TRC_EXCEPT, id, wxID_ANY, \
		(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(TrcExceptEventFunction, &fn), \
		(wxObject *) NULL \
		),

//! trc_debug macros for using in event-map
#define EVT_TRC_DEBUG(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
		wxEVT_TRC_DEBUG, id, wxID_ANY, \
		(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(TrcDebugEventFunction, &fn), \
		(wxObject *) NULL \
		),

//! trc_process macros for using in event-map
#define EVT_TRC_PROCESS(id, fn) \
		DECLARE_EVENT_TABLE_ENTRY( \
		wxEVT_TRC_PROCESS, id, wxID_ANY, \
		(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(TrcProcEventFunction, &fn), \
		(wxObject *) NULL \
		),

//! gui_sndtext macros for using in event-map
#define EVT_GUI_SNDTEXT(id, fn) \
		DECLARE_EVENT_TABLE_ENTRY( \
		wxEVT_GUI_TEXT2WINDOW, id, wxID_ANY, \
		(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(GuiProcEventFunction, &fn), \
		(wxObject *) NULL \
		),
#endif // __EVENTS_H__