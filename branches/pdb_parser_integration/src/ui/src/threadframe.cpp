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

#include "threadframe.h"
#include "debugger.h"

BEGIN_EVENT_TABLE(ThreadFrame, ChildFrame)
	EVT_SIZE(ThreadFrame::OnSize)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
ThreadFrame::ThreadFrame( wxMDIParentFrame *parent, const wxString& title )
 : ChildFrame(parent, title),
   m_listCtrl(this, wxID_ANY,
              wxDefaultPosition, wxDefaultSize,
              wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS)
{
	m_listCtrl.InsertColumn(0, wxT("Ident"));
	m_listCtrl.InsertColumn(1, wxT("Data block"));

	LoadParams();
}

//////////////////////////////////////////////////////////////////////////
ThreadFrame::~ThreadFrame()
{
	SaveParams();
}

//////////////////////////////////////////////////////////////////////////
void ThreadFrame::SaveParams()
{
	SaveWinCoord(wxT("ChildFrame/Thread"));
}

//////////////////////////////////////////////////////////////////////////
void ThreadFrame::LoadParams()
{
	LoadWinCoord(wxT("ChildFrame/Thread"));
}

//////////////////////////////////////////////////////////////////////////
void ThreadFrame::RefreshThreadList( ULONG sesId )
{
	PTHREAD_LIST thread_list = trc_get_thread_list(sesId);

	int szThread = thread_list->count;

	m_listCtrl.DeleteAllItems();


	for (int i = 0; i < szThread; ++i)
	{
		m_listCtrl.InsertItem(i, "");
		m_listCtrl.SetItem(i, 0, wxString::Format(wxT("%X"), thread_list->thread[i].TID));
		m_listCtrl.SetItem(i, 1, wxString::Format(wxT("%X"), thread_list->thread[i].teb_addr));
	}
}

void ThreadFrame::OnSize( wxSizeEvent& event )
{
	m_listCtrl.SetClientSize(event.GetSize());
}