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

/*! \file bpframe.h
* \brief Implementation of CPU frame

This frame provides all functionality for work with code's dump,
break points and some other
* \author d1mk4
*/

#include "precomp.h"

#include "cpuframe.h"

BEGIN_EVENT_TABLE(CPUFrame, ChildFrame)
	EVT_TRC_DEBUG(wxID_ANY, CPUFrame::AddItem)
	EVT_SIZE(CPUFrame::OnSize)
END_EVENT_TABLE()

/*!
CPUFrame constructor
@param[in] parent MDI parent window
@param[in] title  title name for new window
*/
CPUFrame::CPUFrame( wxMDIParentFrame *parent, const wxString& title )
 : ChildFrame(parent, title),
   m_listCtrl(this, CPU_LIST_CTRL,
              wxDefaultPosition, wxDefaultSize,
              wxLC_REPORT | wxSUNKEN_BORDER )
{
	m_listCtrl.InsertColumn(0, wxT("Address"));
	m_listCtrl.InsertColumn(1, wxT("Hex dump"));
	m_listCtrl.InsertColumn(2, wxT("Disassembly"));
	m_listCtrl.InsertColumn(3, wxT("Comment"));
	
	LoadParams();
}

/*!
	Window destructor
*/
CPUFrame::~CPUFrame()
{
	SaveParams();
}

/*!
This handler processes all right button clicks on the listview items
@param[in] event listview event
*/
void CPUFrame::AddItem( TrcDebugEvent& event )
{
	TRC_EXCEPTION_EVENT evt = event.getDebugEvent();
	ULONG sesId = event.getSesId();

	if(evt.EventCode == TRC_EVENT_BREAKPOINT)
	{
		//if(evt->CurrentModule)
		//	printf("mod name %s\n**********",evt->CurrentModule->ModName);
		PMnemonics mnem = DasmDisasmCmd(sesId, (PVOID)evt.Frame.Eip, 0x30);

		m_listCtrl.DeleteAllItems();
		if(mnem)
		{
			//m_listCtrl.SetItemCount(mnem->count);
			for(ULONG i = 0; i < mnem->count; ++i)
			{
				int idx = m_listCtrl.InsertItem(i, mnem->row[i].mnem.AddrStr, 0);
				m_listCtrl.SetItemData(idx, i);
				m_listCtrl.SetItem(idx, 1, mnem->row[i].mnem.HexDump);
				m_listCtrl.SetItem(idx, 2, mnem->row[i].mnem.AsmCode);
			}
			DasmFreeBuffer(mnem);
		}
	}
	else if(evt.EventCode == TRC_EVENT_SINGLE_STEP)
	{
		//if(evt->CurrentModule)
		//	printf("mod name %s\n**********",evt->CurrentModule->ModName);
		//DWORD b=0;
		//printf("single step at address: %x\n",evt->Frame.Eip);
		//_asm int 3;
		//print_regs((ULONG)evt->CurrentThread->TID);
		PMnemonics mnem = DasmDisasmCmd(sesId, (PVOID)evt.Frame.Eip, 0x30);

		m_listCtrl.DeleteAllItems();
		if(mnem)
		{
			//m_listCtrl.SetItemCount(mnem->count);
			for(ULONG i = 0; i < mnem->count; ++i)
			{
				int idx = m_listCtrl.InsertItem(i, mnem->row[i].mnem.AddrStr, 0);
				m_listCtrl.SetItemData(idx, i);
				m_listCtrl.SetItem(idx, 1, mnem->row[i].mnem.HexDump);
				m_listCtrl.SetItem(idx, 2, mnem->row[i].mnem.AsmCode);
			}
			DasmFreeBuffer(mnem);
		}
	}
}

void CPUFrame::ShowContextMenu( const wxPoint& pos )
{
	wxMenu menu;
	menu.Append(wxID_ABOUT, _T("&About"));
	menu.AppendSeparator();
	menu.Append(wxID_EXIT, _T("E&xit"));

	PopupMenu(&menu, pos.x, pos.y);
}

void CPUFrame::OnItemRClick( wxListEvent& event )
{
//	wxMessageBox(m_listCtrl.m_address);
}

void CPUFrame::SaveParams()
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	
	if ( pConfig == NULL )
		return;

	//m_listCtrl.InsertColumn(0, "address");
	//m_listCtrl.InsertColumn(1, "hex dump");
	//m_listCtrl.InsertColumn(2, "disassembly");
	//m_listCtrl.InsertColumn(3, "comment");

	pConfig->Write(wxT("ChildFrame/CPU/width/address"), m_listCtrl.GetColumnWidth(0));
	pConfig->Write(wxT("ChildFrame/CPU/width/hexdump"), m_listCtrl.GetColumnWidth(1));
	pConfig->Write(wxT("ChildFrame/CPU/width/disassembly"), m_listCtrl.GetColumnWidth(2));
	pConfig->Write(wxT("ChildFrame/CPU/width/comment"), m_listCtrl.GetColumnWidth(3));

	this->SaveWinCoord(wxT("ChildFrame/CPU"));
}

void CPUFrame::LoadParams()
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	
	if ( pConfig == NULL )
		return;

	m_listCtrl.SetColumnWidth( 0, pConfig->Read(wxT("ChildFrame/CPU/width/address"), 80) );
	m_listCtrl.SetColumnWidth( 1, pConfig->Read(wxT("ChildFrame/CPU/width/hexdump"), 80) );
	m_listCtrl.SetColumnWidth( 2, pConfig->Read(wxT("ChildFrame/CPU/width/disassembly"), 80) );
	m_listCtrl.SetColumnWidth( 3, pConfig->Read(wxT("ChildFrame/CPU/width/comment"), 80) );

	this->LoadWinCoord(wxT("ChildFrame/CPU"));
}

void CPUFrame::OnTest( wxCommandEvent& event )
{
	wxMessageBox("cpuframe");
}

void CPUFrame::OnSize( wxSizeEvent& event )
{
	m_listCtrl.SetClientSize(event.GetSize());
}
