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

#include "childframe.h"

//////////////////////////////////////////////////////////////////////////
// event map
BEGIN_EVENT_TABLE(ChildFrame, wxControl)
	EVT_MENU(MDI_CHILD_QUIT, ChildFrame::OnQuit)
	EVT_MENU(MDI_REFRESH, ChildFrame::OnRefresh)
	EVT_MENU(MDI_CHANGE_TITLE, ChildFrame::OnChangeTitle)
	EVT_MENU(MDI_CHANGE_POSITION, ChildFrame::OnChangePosition)
	EVT_MENU(MDI_CHANGE_SIZE, ChildFrame::OnChangeSize)

	#if wxUSE_CLIPBOARD
		EVT_MENU(wxID_PASTE, ChildFrame::OnPaste)
		EVT_UPDATE_UI(wxID_PASTE, ChildFrame::OnUpdatePaste)
	#endif // wxUSE_CLIPBOARD

	EVT_SIZE(ChildFrame::OnSize)
	EVT_MOVE(ChildFrame::OnMove)

	EVT_CLOSE(ChildFrame::OnClose)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
//
ChildFrame::ChildFrame(wxMDIParentFrame *parent, const wxString& title)
       : wxControl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                         wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxFRAME_FLOAT_ON_PARENT)
{
    SetSizeHints(100, 100);
}

//////////////////////////////////////////////////////////////////////////
//
ChildFrame::~ChildFrame()
{
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnRefresh(wxCommandEvent& WXUNUSED(event))
{
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnChangePosition(wxCommandEvent& WXUNUSED(event))
{
    Move(10, 10);
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnChangeSize(wxCommandEvent& WXUNUSED(event))
{
	SetClientSize(100, 100);
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_TEXTDLG
	static wxString s_title = _T("Canvas Frame");

	wxString title = wxGetTextFromUser(_T("Enter the new title for MDI child"),
                                     _T("MDI sample question"),
                                     s_title,
                                     GetParent()->GetParent());
	if ( !title )
			return;

	//s_title = title;
	//SetTitle(s_title);
#endif // wxUSE_TEXTDLG
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnActivate(wxActivateEvent& event)
{
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnMove(wxMoveEvent& event)
{
	event.Skip();
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnClose(wxCloseEvent& event)
{
	this->Show(false);
}

#if wxUSE_CLIPBOARD

#include "wx/clipbrd.h"

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    wxClipboardLocker lock;
    wxTextDataObject data;
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::OnUpdatePaste(wxUpdateUIEvent& event)
{
    wxClipboardLocker lock;
    event.Enable( wxTheClipboard->IsSupported(wxDF_TEXT) );
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::SaveWinCoord( const wxString &path )
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	
	if ( pConfig == NULL )
		return;

	// for old MDI concept
	//wxSize size;
	//wxPoint pos;

	//size = GetClientSize();
	//pos = GetPosition();

	//pConfig->Write(path + wxT("/x"), (long) pos.x);
	//pConfig->Write(path + wxT("/y"), (long) pos.y);
	//pConfig->Write(path + wxT("/width"), (long) size.GetX());
	//pConfig->Write(path + wxT("/height"), (long) size.GetY());
}

//////////////////////////////////////////////////////////////////////////
//
void ChildFrame::LoadWinCoord( const wxString &path )
{
	wxConfigBase *pConfig = wxConfigBase::Get();
	
	if ( pConfig == NULL )
		return;

	// for old MDI concept
	//wxSize size;
	//wxPoint pos;

	//pos.x = pConfig->Read(path + wxT("/x"), 50);
	//pos.y = pConfig->Read(path + wxT("/y"), 50);
	//size.SetWidth(pConfig->Read(path + wxT("/width"), 350));
	//size.SetHeight(pConfig->Read(path + wxT("/height"), 200));

	//SetPosition(pos);
	//SetClientSize(size);
}
#endif // wxUSE_CLIPBOARD
