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

#include "memframe.h"
#include "utils.h"

BEGIN_EVENT_TABLE(MemFrame, ChildFrame)
	EVT_TRC_DEBUG(wxID_ANY, MemFrame::RefreshMemory)
	EVT_SIZE(MemFrame::OnSize)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// ctor
MemFrame::MemFrame( wxMDIParentFrame *parent, const wxString& title )
 : ChildFrame(parent, title)
{
	m_panel = new wxPanel(this);
	m_sizerFrame = new wxBoxSizer(wxVERTICAL);
	m_memctrl = new ListTextCtrl(m_panel);

	m_sizerFrame->Add(m_memctrl, 1, wxEXPAND);
	m_sizerFrame->Layout();
	m_panel->SetSizer(m_sizerFrame);

	LoadParams();
}

//////////////////////////////////////////////////////////////////////////
// dtor
MemFrame::~MemFrame()
{
	SaveParams();
}

//////////////////////////////////////////////////////////////////////////
//
void MemFrame::SaveParams()
{
	this->SaveWinCoord(wxT("ChildFrame/Mem"));
}

//////////////////////////////////////////////////////////////////////////
//
void MemFrame::LoadParams()
{
	this->LoadWinCoord(wxT("ChildFrame/Mem"));
}

void MemFrame::RefreshMemory( TrcDebugEvent& event )
{
	CONTEXT ctx;
	PTHREAD_LIST threadList = trc_get_thread_list(event.getSesId());
	trc_get_thread_ctx(event.getSesId(), threadList->thread[0].TID, &ctx);
	OutputMemory(event.getSesId(), ctx.Eip);
}

void MemFrame::OutputMemory( uint32_t sesId, uint32_t mem_addr )
{
	wxString output;

	int szMemRegion = 1024;
	int k = 0, align = 16, padding = 0, pktSize = szMemRegion;

	unsigned char *buff = new unsigned char[pktSize];
	unsigned char *p = buff;
	trc_read_memory(sesId, (PVOID)addr, buff, pktSize);

	while(pktSize)
	{
		if(pktSize < 16)
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

	m_memctrl->SetAddrValue(addr, szMemRegion);
	m_memctrl->SetDumpValue(output);

	delete[] buff;
}

void MemFrame::OnSize( wxSizeEvent& event )
{
	m_panel->SetClientSize(event.GetSize());
}
