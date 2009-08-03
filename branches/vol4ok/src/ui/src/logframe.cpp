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

#include "precomp.h"

#include "logframe.h"

BEGIN_EVENT_TABLE(LogFrame, ChildFrame)
	EVT_TRC_EXCEPT(wxID_ANY, LogFrame::ExceptEventToLog)
	EVT_TRC_DEBUG(wxID_ANY, LogFrame::DbgEventToLog)
	EVT_TRC_PROCESS(wxID_ANY, LogFrame::ProcEventToLog)
	EVT_SIZE(LogFrame::OnSize)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
LogFrame::LogFrame( wxMDIParentFrame *parent, const wxString& title )
  : ChildFrame(parent, title),
    m_textCtrl(this, wxID_ANY, wxEmptyString,
               wxDefaultPosition, wxDefaultSize,
               wxTE_MULTILINE | wxTE_READONLY),
    m_counter(0)
{
	this->LoadParams();
	wxLog::SetActiveTarget(new wxLogTextCtrl(&m_textCtrl));
}

//////////////////////////////////////////////////////////////////////////
LogFrame::~LogFrame()
{
	this->SaveParams();
}

//////////////////////////////////////////////////////////////////////////
void LogFrame::Print( const wxString &str )
{
	m_textCtrl.AppendText(str);
}

//////////////////////////////////////////////////////////////////////////
void LogFrame::DbgEventToLog( TrcDebugEvent &event )
{
	wxString output;
	TRC_EXCEPTION_EVENT evt = event.getDebugEvent();

	//output.Printf("current eip: 0x%0.8X\n", evt.Frame.Eip);
	//this->Print(output);
}

//////////////////////////////////////////////////////////////////////////
void LogFrame::ExceptEventToLog( TrcExceptEvent &event )
{
	wxString output;
	TRC_EXCEPTION_EVENT evt = event.getExceptEvent();

	output.Printf("event code: %#x, EIP = %#x\n", evt.EventCode, evt.Frame.Eip);

	this->Print(wxString(_T("ExceptEvt: ")) + output);
}

//////////////////////////////////////////////////////////////////////////
void LogFrame::ProcEventToLog( TrcProcEvent &event )
{
	wxString output;
	TRC_PROCESS_EVENT evt = event.getProcEvent();

	if(evt.EventCode == TRC_EVENT_TERMINATE)
	{
		output.Printf(wxT("process terminated\n"));
	}
	else if(evt.EventCode == TRC_EVENT_THREAD_START)
	{
		output.Printf(wxT("new thread started thread id is 0x%0.8X thread teb is %x\n"),
			evt.StartedThread->TID, evt.StartedThread->teb_addr);
	}
	else if(evt.EventCode == TRC_EVENT_THREAD_EXIT)
	{
		output.Printf(wxT("thread exited thread id is 0x%0.8X exit code is %x\n"), evt.ExitedThread->TID,
			evt.ExitedThread->exit_code);
	}
	else if(evt.EventCode == TRC_EVENT_LOAD_MODULE)
	{
		
		output.Printf(wxT("module %s loaded,image Base is 0x%0.8X size is %x\n"), evt.LoadedModule->ModName,
			evt.LoadedModule->ImageBase, evt.LoadedModule->ImageSize);
	}
	else if(evt.EventCode == TRC_EVENT_UNLOAD_MODULE)
	{
		output.Printf(wxT("module %s unloaded,image Base is 0x%0.8X size is %x\n"), evt.UnloadedModule->ModName,
			evt.UnloadedModule->ImageBase, evt.UnloadedModule->ImageSize);
	}

	wxDynamicCast(GetParent(), wxFrame)->SetStatusText(output);
}

//////////////////////////////////////////////////////////////////////////
void LogFrame::SaveParams()
{
	this->SaveWinCoord(wxT("ChildFrame/Log"));
}

//////////////////////////////////////////////////////////////////////////
void LogFrame::LoadParams()
{
	this->LoadWinCoord(wxT("ChildFrame/Log"));
}

//////////////////////////////////////////////////////////////////////////
void LogFrame::OnSize( wxSizeEvent& event )
{
	m_textCtrl.SetClientSize(event.GetSize());
}