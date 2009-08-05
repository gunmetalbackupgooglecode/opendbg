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

#include "bpframe.h"
#include "utils.h"
#include "events.h"
#include "listview.h"

//////////////////////////////////////////////////////////////////////////
//
BEGIN_EVENT_TABLE(BPFrame, ChildFrame)
	EVT_GUI_SNDTEXT(GUI_MSG_ADD_BP, BPFrame::OnAddBP)
	EVT_GUI_SNDTEXT(GUI_MSG_DEL_BP, BPFrame::OnDelBP)
	EVT_SIZE(BPFrame::OnSize)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// 
BPFrame::BPFrame( wxMDIParentFrame *parent, const wxString& title )
 : ChildFrame(parent, title),
   m_listCtrl(this, wxID_ANY,
              wxDefaultPosition, wxDefaultSize,
              wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS)
{
	m_listCtrl.InsertColumn(0, wxT("Address"));
	m_listCtrl.InsertColumn(1, wxT("Module"));
	m_listCtrl.InsertColumn(2, wxT("Active"));
	m_listCtrl.InsertColumn(3, wxT("Disassembly"));
	m_listCtrl.InsertColumn(4, wxT("Comment"));

	this->LoadParams();
}

//////////////////////////////////////////////////////////////////////////
// 
BPFrame::~BPFrame()
{
	this->SaveParams();
}

//////////////////////////////////////////////////////////////////////////
//
void BPFrame::SaveParams()
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	
	if ( pConfig == NULL )
		return;

	pConfig->Write(wxT("ChildFrame/BP/width/address"), m_listCtrl.GetColumnWidth(0));
	pConfig->Write(wxT("ChildFrame/BP/width/module"), m_listCtrl.GetColumnWidth(1));
	pConfig->Write(wxT("ChildFrame/BP/width/active"), m_listCtrl.GetColumnWidth(2));
	pConfig->Write(wxT("ChildFrame/BP/width/disassembly"), m_listCtrl.GetColumnWidth(3));
	pConfig->Write(wxT("ChildFrame/BP/width/comment"), m_listCtrl.GetColumnWidth(4));

	this->SaveWinCoord(wxT("ChildFrame/BP"));
}

//////////////////////////////////////////////////////////////////////////
//
void BPFrame::LoadParams()
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	
	if ( pConfig == NULL )
		return;

	m_listCtrl.SetColumnWidth( 0, pConfig->Read(wxT("ChildFrame/BP/width/address"), 80) );
	m_listCtrl.SetColumnWidth( 1, pConfig->Read(wxT("ChildFrame/BP/width/module"), 80) );
	m_listCtrl.SetColumnWidth( 2, pConfig->Read(wxT("ChildFrame/BP/width/active"), 80) );
	m_listCtrl.SetColumnWidth( 3, pConfig->Read(wxT("ChildFrame/BP/width/disassembly"), 80) );
	m_listCtrl.SetColumnWidth( 4, pConfig->Read(wxT("ChildFrame/BP/width/comment"), 80));

	this->LoadWinCoord(wxT("ChildFrame/BP"));
}

//////////////////////////////////////////////////////////////////////////
//
void BPFrame::OnAddBP( GuiProcEvent& event )
{
	PBP_LST bp_list = trc_get_bp_list(event.getSesId(),
	(ULONG)event.getDebugEvent().CurrentThread->TID);

	m_listCtrl.DeleteAllItems(); // очищаем список
	for (ULONG i = 0; i < bp_list->count; ++i)
	{
		m_listCtrl.InsertItem(i, wxT(""));
		m_listCtrl.SetItem( i, 0, wxString::Format(wxT("%X"), bp_list->bp[i].addr) );
		m_listCtrl.SetItem( i, 1, wxString::Format(wxT("%s"), event.getDebugEvent().CurrentModule->ModName));
	}
}

//////////////////////////////////////////////////////////////////////////
//
void BPFrame::OnDelBP( GuiProcEvent& event )
{
	//PBP_LST bp_list = trc_get_bp_list(event.getSesId(),
	//(ULONG)event.getDebugEvent().CurrentThread->TID);

	//m_listCtrl.DeleteAllItems(); // очищаем список
	//for (size_t i = 0; i < bp_list->count; ++i)
	//{
	//	m_listCtrl.InsertItem(i, "");
	//	m_listCtrl.SetItem( i, 0, wxString::Format("%X", bp_list->bp[i].addr) );
	//	m_listCtrl.SetItem( i, 1, event.getDebugEvent().CurrentModule->ModName);
	//}
}

void BPFrame::OnSize( wxSizeEvent& event )
{
	m_listCtrl.SetClientSize(event.GetSize());
}