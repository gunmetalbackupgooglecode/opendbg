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

#include "stackframe.h"
#include "utils.h"

BEGIN_EVENT_TABLE(StackFrame, ChildFrame)
	EVT_TRC_DEBUG(wxID_ANY, StackFrame::RefreshMemory)
	EVT_SIZE(StackFrame::OnSize)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// ctor
StackFrame::StackFrame( wxMDIParentFrame *parent, const wxString& title )
 : ChildFrame(parent, title),
   m_stackctrl(this)
{
	LoadParams();
}

//////////////////////////////////////////////////////////////////////////
// dtor
StackFrame::~StackFrame()
{
	SaveParams();
}

//////////////////////////////////////////////////////////////////////////
//
void StackFrame::SaveParams()
{
	this->SaveWinCoord(wxT("ChildFrame/Stack"));
}

//////////////////////////////////////////////////////////////////////////
//
void StackFrame::LoadParams()
{
	this->LoadWinCoord(wxT("ChildFrame/Stack"));
}

void StackFrame::RefreshMemory( TrcDebugEvent& event )
{
	CONTEXT ctx;
	PTHREAD_LIST threadList = trc_get_thread_list(event.getSesId());
	trc_get_thread_ctx(event.getSesId(), threadList->thread[0].TID, &ctx);
	OutputMemory(event.getSesId(), ctx.Ebp);
}

void StackFrame::OutputMemory( ULONG sesId, ULONG addr )
{
	wxString output;

	addr -= 1024;
	int szMemRegion = 1024;
	int k = 0, align = 16, padding = 0, pktSize = szMemRegion;
	unsigned char *buff = new unsigned char[pktSize];
	unsigned char *p = buff;
	trc_read_memory(sesId, (PVOID)addr, buff, pktSize);

	while(pktSize)
	{
		if(pktSize<16)
		{
			align = pktSize;
			padding = 16 - pktSize;
		}
		
		for(int j = 0; j < align; j++, k++)
		{
			// unshowable chars
			if( (p[k] != 173) && (p[k] > 31 && p[k] < 127 || p[k] > 159) )
				output.Append( wxString::Format( wxT("%c"), p[k]) );
			else
				output.Append( wxString::Format(wxT(".")) ); //Special Character '?'
		}

		pktSize -= align;
	}

	m_stackctrl.SetAddrValue(addr, szMemRegion);
	m_stackctrl.SetDumpValue(output);

	delete buff;
}

void StackFrame::OnSize( wxSizeEvent& event )
{
	m_stackctrl.SetClientSize(event.GetSize());
}