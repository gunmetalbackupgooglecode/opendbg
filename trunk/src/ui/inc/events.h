#ifndef __EVENTS_H__
#define __EVENTS_H__

#if _MSC_VER > 1000 
    #pragma once  
#endif // _MSC_VER > 1000

#include "precomp.h"
#include "debugger.h"

//////////////////////////////////////////////////////////////////////////
// Event class except_event
class TrcExceptEvent : public wxNotifyEvent
{
public:
	// default constructor
	TrcExceptEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id)
	{
	}

	// constructor via sesId
	TrcExceptEvent(TRC_EXCEPTION_EVENT except_evt, ULONG sesId,
	                  wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id),
	   m_except_event(except_evt),
	   m_sesId(sesId)
	{
	}

	// copy constructor
	// should be used to pass event with additional data among diffrent threads
	TrcExceptEvent(const TrcExceptEvent& event)
	 : wxNotifyEvent(event)
	{
		this->m_except_event = event.m_except_event;
		this->m_sesId = event.m_sesId;
	}

	// object clone function
	virtual wxEvent *Clone() const
	{
		return new TrcExceptEvent(*this);
	};

	// Getters
	TRC_EXCEPTION_EVENT getExceptEvent();
	ULONG getSesId();

	// Setters
	void setExceptEvent(const TRC_EXCEPTION_EVENT& except_event);
	void setSesId(const ULONG &sesId);

private:
	// debug event TRC_EXCEPTION_EVENT
	TRC_EXCEPTION_EVENT m_except_event;
	// debugger session ID
	ULONG m_sesId;

private:
	DECLARE_DYNAMIC_CLASS(TrcExceptEvent)
};

//////////////////////////////////////////////////////////////////////////
// Класс событий dbg_event
class TrcDebugEvent : public wxNotifyEvent
{
public:
	// конструктор по умолчанию
	TrcDebugEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
  : wxNotifyEvent(commandType, id)
	{
	}

	// конструктор в котором можно задать событие и sesId
	TrcDebugEvent(TRC_EXCEPTION_EVENT debug_evt, ULONG sesId,
	                  wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id),
	   m_debug_event(debug_evt),
	   m_sesId(sesId)
	{
	}

	// конструктор копирования
	// без него хуй эти сообщения будут передаваться из потока в поток
	// с дополнительными данными
	TrcDebugEvent(const TrcDebugEvent& event)
	 : wxNotifyEvent(event)
	{
		this->m_debug_event = event.m_debug_event;
		this->m_sesId = event.m_sesId;
	}

	// функция для копирования объекта в новый
	virtual wxEvent *Clone() const
	{
		return new TrcDebugEvent(*this);
	};

	// геттеры
	TRC_EXCEPTION_EVENT getDebugEvent();
	ULONG getSesId();

	// сеттеры
	void setDebugEvent(const TRC_EXCEPTION_EVENT& debug_event);
	void setSesId(const ULONG& sesId);

private:
	TRC_EXCEPTION_EVENT m_debug_event; // событие отлачдика TRC_EXCEPTION_EVENT
	ULONG m_sesId; // идентификатор сессии отладчика

private:
	DECLARE_DYNAMIC_CLASS(TrcDebugEvent);
};

//////////////////////////////////////////////////////////////////////////
// Класс событий process_event
class TrcProcEvent : public wxNotifyEvent
{
public:
	// конструктор по умолчанию
	TrcProcEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id)
	{
	}

	// конструктор копирования
	// без него хуй эти сообщения будут передаваться из потока в поток
	// с дополнительными данными
	TrcProcEvent(const TrcProcEvent& event)
	 : wxNotifyEvent(event)
	{
		this->m_proc_event = event.m_proc_event;
		this->m_sesId = event.m_sesId;
	}

	// конструктор в котором можно задать событие и sesId
	TrcProcEvent(TRC_PROCESS_EVENT proc_evt, ULONG sesId,
	                  wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id),
	   m_proc_event(proc_evt),
	   m_sesId(sesId)
	{
	}

	// функция для копирования объекта в новый
	virtual wxEvent *Clone() const
	{
		return new TrcProcEvent(*this);
	};

	// геттеры
	TRC_PROCESS_EVENT getProcEvent();
	ULONG getSesId();

	// сеттеры
	void setProcEvent(const TRC_PROCESS_EVENT &proc_event);
	void setSesId(const ULONG& sesId);

private:
	TRC_PROCESS_EVENT m_proc_event; // событие отлачдика TRC_PROCESS_EVENT
	ULONG m_sesId; // идентификатор сессии отладчика

private:
	DECLARE_DYNAMIC_CLASS(TrcProcEvent);
};

class GuiProcEvent : public wxNotifyEvent
{
public:
	// конструктор по умолчанию
	GuiProcEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
	 : wxNotifyEvent(commandType, id)
	{
	}

	// конструктор копирования
	// без него хуй эти сообщения будут передаваться из потока в поток
	// с дополнительными данными
	GuiProcEvent(const GuiProcEvent& event)
	 : wxNotifyEvent(event)
	{
		this->m_addrBP = event.m_addrBP;
		this->m_disasmBP = event.m_disasmBP;
		this->m_debug_event = event.m_debug_event;
		this->m_sesId = event.m_sesId;
	}

	// сеттеры
	void setAddrBP(const wxString &addrBP);
	void setDisasmBP(const wxString &disasmBP);
	void setDebugEvent(const TRC_EXCEPTION_EVENT &debug_event);
	void setSesId(const ULONG& sesId);

	// геттеры
	wxString getAddrBP();
	wxString getDisasmBP();
	TRC_EXCEPTION_EVENT getDebugEvent();
	ULONG getSesId();

	// конструктор в котором можно задать событие и sesId
	//GuiProcEvent(TRC_PROCESS_EVENT proc_evt, ULONG sesId,
	//                  wxEventType commandType = wxEVT_NULL, int id = 0)
	// : wxNotifyEvent(commandType, id),
	//{
	//}

	// функция для копирования объекта в новый
	virtual wxEvent *Clone() const
	{
		return new GuiProcEvent(*this);
	};

private:
	wxString m_addrBP;
	wxString m_disasmBP;
	TRC_EXCEPTION_EVENT m_debug_event; // событие отлачдика TRC_PROCESS_EVENT
	ULONG m_sesId; // идентификатор сессии отладчика

private:
	DECLARE_DYNAMIC_CLASS(GuiProcEvent);
};

// определяем функции(конструкторы) для обработчика событий
typedef void (wxEvtHandler::*TrcExceptEventFunction)(TrcExceptEvent&);
typedef void (wxEvtHandler::*TrcDebugEventFunction)(TrcDebugEvent&);
typedef void (wxEvtHandler::*TrcProcEventFunction)(TrcProcEvent&);

typedef void (wxEvtHandler::*GuiProcEventFunction)(GuiProcEvent&);

// определяем номера наших событий
BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE(wxEVT_TRC_EXCEPT, 7776)
	DECLARE_EVENT_TYPE(wxEVT_TRC_DEBUG, 7777)
	DECLARE_EVENT_TYPE(wxEVT_TRC_PROCESS, 7778)

	DECLARE_EVENT_TYPE(wxEVT_GUI_TEXT2WINDOW, 8888)
END_DECLARE_EVENT_TYPES()

enum
{
	GUI_MSG_ADD_BP = 10001,
	GUI_MSG_DEL_BP
};

// определяем макросы для использования в карте событий
#define EVT_TRC_EXCEPT(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
		wxEVT_TRC_EXCEPT, id, wxID_ANY, \
		(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(TrcExceptEventFunction, &fn), \
		(wxObject *) NULL \
		),

#define EVT_TRC_DEBUG(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( \
		wxEVT_TRC_DEBUG, id, wxID_ANY, \
		(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(TrcDebugEventFunction, &fn), \
		(wxObject *) NULL \
		),

#define EVT_TRC_PROCESS(id, fn) \
		DECLARE_EVENT_TABLE_ENTRY( \
		wxEVT_TRC_PROCESS, id, wxID_ANY, \
		(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(TrcProcEventFunction, &fn), \
		(wxObject *) NULL \
		),

#define EVT_GUI_SNDTEXT(id, fn) \
		DECLARE_EVENT_TABLE_ENTRY( \
		wxEVT_GUI_TEXT2WINDOW, id, wxID_ANY, \
		(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(GuiProcEventFunction, &fn), \
		(wxObject *) NULL \
		),
#endif // __EVENTS_H__