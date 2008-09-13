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

#include "modframe.h"
#include "debugger.h"

//////////////////////////////////////////////////////////////////////////
//
BEGIN_EVENT_TABLE(ModFrame, ChildFrame)
	EVT_SIZE(ModFrame::OnSize)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
// конструктор
ModFrame::ModFrame( wxMDIParentFrame *parent, const wxString& title )
 : ChildFrame(parent, title),
   m_listCtrl(this, wxID_ANY,
              wxDefaultPosition, wxDefaultSize,
              wxLC_REPORT | wxSUNKEN_BORDER | wxLC_EDIT_LABELS)
{
	m_listCtrl.InsertColumn(0, wxT("Base"));
	m_listCtrl.InsertColumn(1, wxT("Size"));
	m_listCtrl.InsertColumn(2, wxT("Entry"));
	m_listCtrl.InsertColumn(3, wxT("Name"));
	m_listCtrl.InsertColumn(4, wxT("File version"));
	m_listCtrl.InsertColumn(5, wxT("Path"));

	this->LoadParams();
}

//////////////////////////////////////////////////////////////////////////
// Деструктор
ModFrame::~ModFrame()
{
	this->SaveParams();
}

//////////////////////////////////////////////////////////////////////////
//
void ModFrame::SaveParams() 
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	
	if ( pConfig == NULL )
		return;

	pConfig->Write(wxT("ChildFrame/Mod/width/base"), m_listCtrl.GetColumnWidth(0));
	pConfig->Write(wxT("ChildFrame/Mod/width/size"), m_listCtrl.GetColumnWidth(1));
	pConfig->Write(wxT("ChildFrame/Mod/width/entry"), m_listCtrl.GetColumnWidth(2));
	pConfig->Write(wxT("ChildFrame/Mod/width/name"), m_listCtrl.GetColumnWidth(3));
	pConfig->Write(wxT("ChildFrame/Mod/width/filever"), m_listCtrl.GetColumnWidth(4));
	pConfig->Write(wxT("ChildFrame/Mod/width/path"), m_listCtrl.GetColumnWidth(5));

	this->SaveWinCoord(wxT("ChildFrame/Mod"));
}

//////////////////////////////////////////////////////////////////////////
//
void ModFrame::LoadParams()
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	
	if ( pConfig == NULL )
		return;

	m_listCtrl.SetColumnWidth( 0, pConfig->Read(wxT("ChildFrame/Mod/width/base"), 80) );
	m_listCtrl.SetColumnWidth( 1, pConfig->Read(wxT("ChildFrame/Mod/width/size"), 80) );
	m_listCtrl.SetColumnWidth( 2, pConfig->Read(wxT("ChildFrame/Mod/width/entry"), 80) );
	m_listCtrl.SetColumnWidth( 3, pConfig->Read(wxT("ChildFrame/Mod/width/name"), 80) );
	m_listCtrl.SetColumnWidth( 4, pConfig->Read(wxT("ChildFrame/Mod/width/filever"), 80) );
	m_listCtrl.SetColumnWidth( 5, pConfig->Read(wxT("ChildFrame/Mod/width/path"), 80));

	this->LoadWinCoord(wxT("ChildFrame/Mod"));
}

//////////////////////////////////////////////////////////////////////////
//
void ModFrame::RefreshModList(ULONG sesId)
{
	PMOD_LIST mod_list = trc_get_module_list(sesId);

	m_listCtrl.DeleteAllItems();
	for (unsigned long i = 0; i < mod_list->count; ++i)
	{
		m_listCtrl.InsertItem(i, "");
		m_listCtrl.SetItem(i, 0, wxString::Format(wxT("%X"), mod_list->module[i].ImageBase));
		m_listCtrl.SetItem(i, 1, wxString::Format(wxT("%i"), mod_list->module[i].ImageSize));
		m_listCtrl.SetItem(i, 5, mod_list->module[i].ModName);
	}
}

void ModFrame::OnSize( wxSizeEvent& event )
{
	m_listCtrl.SetClientSize(event.GetSize());
}