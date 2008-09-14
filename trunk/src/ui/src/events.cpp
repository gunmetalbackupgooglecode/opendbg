/*
    *    
    * Copyright (c) 2008 
    * d1mk4 <d1mk4@bk.ru> 
    *

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*! \file events.cpp
* \brief Implementation of debugger's events
* \author d1mk4
*/

#include "precomp.h"
#include "events.h"

DEFINE_EVENT_TYPE(wxEVT_TRC_EXCEPT)
DEFINE_EVENT_TYPE(wxEVT_TRC_DEBUG)
DEFINE_EVENT_TYPE(wxEVT_TRC_PROCESS)

DEFINE_EVENT_TYPE(wxEVT_GUI_TEXT2WINDOW)

IMPLEMENT_DYNAMIC_CLASS(TrcExceptEvent, wxNotifyEvent)
IMPLEMENT_DYNAMIC_CLASS(TrcDebugEvent, wxNotifyEvent)
IMPLEMENT_DYNAMIC_CLASS(TrcProcEvent, wxNotifyEvent)

IMPLEMENT_DYNAMIC_CLASS(GuiProcEvent, wxNotifyEvent)

TRC_EXCEPTION_EVENT TrcExceptEvent::getExceptEvent()
{
	return m_except_event;
}

ULONG TrcExceptEvent::getSesId()
{
	return m_sesId;
}

void TrcExceptEvent::setExceptEvent( const TRC_EXCEPTION_EVENT& except_event )
{
	m_except_event = except_event;
}

void TrcExceptEvent::setSesId( const ULONG& sesId )
{
	m_sesId = sesId;
}

TRC_EXCEPTION_EVENT TrcDebugEvent::getDebugEvent()
{
	return m_debug_event;
}

ULONG TrcDebugEvent::getSesId()
{
	return m_sesId;
}

void TrcDebugEvent::setDebugEvent( const TRC_EXCEPTION_EVENT& debug_event )
{
	m_debug_event = debug_event;
}

void TrcDebugEvent::setSesId( const ULONG& sesId )
{
	m_sesId = sesId;
}

TRC_PROCESS_EVENT TrcProcEvent::getProcEvent()
{
	return m_proc_event;
}

ULONG TrcProcEvent::getSesId()
{
	return m_sesId;
}

void TrcProcEvent::setProcEvent( const TRC_PROCESS_EVENT &proc_event )
{
	m_proc_event = proc_event;
}

void TrcProcEvent::setSesId( const ULONG& sesId )
{
	m_sesId = sesId;
}

wxString GuiProcEvent::getAddrBP()
{
	return this->m_addrBP;
}

wxString GuiProcEvent::getDisasmBP()
{
	return this->m_disasmBP;
}

TRC_EXCEPTION_EVENT GuiProcEvent::getDebugEvent()
{
	return this->m_debug_event;
}

ULONG GuiProcEvent::getSesId()
{
	return this->m_sesId;
}

void GuiProcEvent::setAddrBP( const wxString &addrBP )
{
	this->m_addrBP = addrBP;
}

void GuiProcEvent::setDisasmBP( const wxString &disasmBP )
{
	this->m_disasmBP = disasmBP;
}

void GuiProcEvent::setDebugEvent( const TRC_EXCEPTION_EVENT &debug_event )
{
	this->m_debug_event = debug_event;
}

void GuiProcEvent::setSesId( const ULONG& sesId )
{
	this->m_sesId = sesId;
}
