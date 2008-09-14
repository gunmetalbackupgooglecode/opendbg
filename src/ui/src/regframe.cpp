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

#include "regframe.h"

static const char *RegistersName[] = { wxT("EAX"), wxT("ECX"), wxT("EDX"), wxT("EBX"), wxT("ESP"),
                                  wxT("EBP"), wxT("ESI"), wxT("EDI"), wxT("EIP"),
                                  wxT("ES"), wxT("CS"), wxT("SS"), wxT("DS"), wxT("FS"), wxT("GS") };

BEGIN_EVENT_TABLE(RegFrame, ChildFrame)
	EVT_TRC_DEBUG(wxID_ANY, RegFrame::Output)
	EVT_SIZE(RegFrame::OnSize)

END_EVENT_TABLE()

BEGIN_EVENT_TABLE(RegRichTextCtrl, wxRichTextCtrl)
	EVT_LEFT_DOWN(RegRichTextCtrl::OnLeftClick)
	EVT_LEFT_DCLICK(RegRichTextCtrl::OnLeftDClick)
	EVT_CHAR(RegRichTextCtrl::OnChar)
	EVT_MOTION(RegRichTextCtrl::OnMoveMouse)
END_EVENT_TABLE()

RegRichTextCtrl::RegRichTextCtrl( wxWindow* parent )
  : wxRichTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition,
    wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_NO_VSCROLL),
    m_sesId(-1)
{
	SetCaret(0);
	SetCursor(wxCURSOR_ARROW);
}

void RegRichTextCtrl::OnLeftClick( wxMouseEvent& event )
{
	SetFocus();

	wxClientDC dc(this);
	PrepareDC(dc);
	dc.SetFont(GetFont());

	long position = 0;
	int hit = GetBuffer().HitTest(dc, event.GetLogicalPosition(dc), position);

	if (hit != wxRICHTEXT_HITTEST_NONE)
	{
		bool caretAtLineStart = false;

		if (hit & wxRICHTEXT_HITTEST_BEFORE)
		{
			// If we're at the start of a line (but not first in para)
			// then we should keep the caret showing at the start of the line
			// by showing the m_caretAtLineStart flag.
			wxRichTextParagraph* para = GetBuffer().GetParagraphAtPosition(position);
			wxRichTextLine* line = GetBuffer().GetLineAtPosition(position);

			if (line && para &&
				line->GetAbsoluteRange().GetStart() == position
				&& para->GetRange().GetStart() != position)
					caretAtLineStart = true;
			position--;
		}
	
		MoveCaret(position, caretAtLineStart);
		SelectWord(GetCaretPosition());
	}
}

// TODO: this code require full information about editabel register in dialog
// TODO: to make editable all registers which is output
void RegRichTextCtrl::OnLeftDClick(wxMouseEvent& event)
{
	wxClientDC dc(this);
	PrepareDC(dc);
	dc.SetFont(GetFont());

	long position = 0;
	GetBuffer().HitTest(dc, event.GetLogicalPosition(dc), position);
	long linePos = GetBuffer().GetVisibleLineNumber(position, true, true);
	wxString line = GetLineText(linePos);

	const size_t szRegNames = 15;

	for (size_t i = 0; i < szRegNames; ++i)
		if (line.Find(RegistersName[i]) != wxNOT_FOUND)
	{
		wxString str = GetStringSelection();

		bool bShowEditBox = true;

		for (size_t k = 0; k < szRegNames; ++k)
		{
			if ( 0 == str.Cmp(RegistersName[k]) )
			{
				bShowEditBox = false;
				break;
			}
		}

		if (bShowEditBox)
		{
			EditRegister(RegistersName[i], str);
			break;
		}
	}
}

void RegRichTextCtrl::OnChar( wxKeyEvent& event )
{
	long linePos = GetBuffer().GetVisibleLineNumber(GetCaretPosition(), true, true);
	wxString line = GetLineText(linePos);
	
	for (size_t i = 0; i < 15; ++i)
		if ( wxNOT_FOUND != line.Find(RegistersName[i]) )
	{
		wxString str = GetStringSelection();

		bool bShowEditBox = true;
		for (size_t k = 0; k < 15; ++k)
		{
			if ( 0 == str.Cmp(RegistersName[k]) )
			{
				bShowEditBox = false;
				break;
			}
		}

		if (bShowEditBox)
		{
			this->SetCaretPosition(GetCaretPosition(), true);
			//EditRegister(RegNames[i], str);
			break;
		}
	}
}

void RegRichTextCtrl::SetContextId( int sesId )
{
	m_sesId = sesId;
}

int RegRichTextCtrl::GetContextId()
{
	return m_sesId;
}

void RegRichTextCtrl::SetThreadId( int tid )
{
	m_tid = tid;
}

int RegRichTextCtrl::GetThreadId()
{
	return m_tid;
}

//////////////////////////////////////////////////////////////////////////
// output all basic registers, segment registers and EFLAGS
void RegRichTextCtrl::OutputBasic( const Context &ctx )
{
	wxString regs, segs, eflags, output;

	// forming basic registers value for output to registers window
	regs.Printf(wxT("EAX\t%0.8X\nECX\t%0.8X\nEDX\t%0.8X\nEBX\t%0.8X\nESP\t%0.8X\n"
	                "EBP\t%0.8X\nESI\t%0.8X\nEDI\t%0.8X\n\nEIP\t%0.8X\n\n"),
	ctx.Eax, ctx.Ecx, ctx.Edx, ctx.Ebx, ctx.Esp, ctx.Ebp, ctx.Esi, ctx.Edi, ctx.Eip);
	
	// forming segment registers value for output to registers window
	segs.Printf(wxT("ES\t%0.8X\nCS\t%0.8X\nSS\t%0.8X\nDS\t%0.8X\nFS\t%0.8X\nGS\t%0.8X\n\n"),
	ctx.SegEs, ctx.SegCs, ctx.SegSs, ctx.SegDs, ctx.SegFs, ctx.SegGs);

	// forming flags register value for output to window
	eflags.Printf(wxT("EFL\t%0.8X"), ctx.EFlags);

	output.Append(regs);
	output.Append(segs);
	output.Append(eflags);

	SetValue(output);
}

//////////////////////////////////////////////////////////////////////////
// output values Debug Registers
void RegRichTextCtrl::OutputDR( const Context &ctx )
{
	wxString dbgRegs;
	
	dbgRegs.Printf(wxT("DR0\t%0.8X\nDR1\t%0.8X\nDR2\t%0.8X\nDR3\t%0.8X\n"
		                 "DR6\t%0.8X\nDR7\t%0.8X"),
		             ctx.Dr0, ctx.Dr1, ctx.Dr2, ctx.Dr3, ctx.Dr6, ctx.Dr7);

	SetValue(dbgRegs);
}

void RegRichTextCtrl::EditRegister( const char *RegNames, const wxString &value )
{
	unsigned long tmp;
	value.ToULong(&tmp, 16);

	wxString newStr = wxGetTextFromUser(RegNames, wxT("Edit register value"),
				wxString::Format(wxT("%X"), tmp), this);

	// if user press cancel then newStr will be empty
	if (!newStr.empty())
	{
		Context ctx;
		trc_get_thread_ctx(m_sesId, m_tid, &ctx);

		newStr.ToULong(&tmp, 16);

		WhatBasicRegister(RegNames, tmp, ctx);

		// TODO: to complete output for all warnings and errors to wxLog
		if (0 == trc_set_thread_ctx(m_sesId, m_tid, &ctx) )
			wxLogError(wxString(wxT("Can't set ")) + RegNames + wxString(wxT(" value")));
		Context ctx_new;
		if (0 == trc_get_thread_ctx(m_sesId, m_tid, &ctx_new) )
			wxLogError(wxString(wxT("Can't get ")) + RegNames + wxString(wxT(" value")));

		OutputBasic(ctx_new);
	}
}

void RegRichTextCtrl::WhatBasicRegister( const char *RegNames, int val, Context &ctx )
{
	if ( 0 == wxStricmp(RegNames, wxT("EAX")) )
		ctx.Eax = val;
	else if ( 0 == wxStricmp(RegNames, wxT("ECX")) )
		ctx.Ecx = val;
	else if ( 0 == wxStricmp(RegNames, wxT("EDX")) )
		ctx.Edx = val;
	else if ( 0 == wxStricmp(RegNames, wxT("EBX")) )
		ctx.Ebx = val;
	else if ( 0 == wxStricmp(RegNames, wxT("ESP")) )
		ctx.Esp = val;
	else if ( 0 == wxStricmp(RegNames, wxT("EBP")) )
		ctx.Ebp = val;
	else if ( 0 == wxStricmp(RegNames, wxT("ESI")) )
		ctx.Esi = val;
	else if ( 0 == wxStricmp(RegNames, wxT("EDI")) )
		ctx.Edi = val;
	else if ( 0 == wxStricmp(RegNames, wxT("EIP")) )
		ctx.Eip = val;
	else if ( 0 == wxStricmp(RegNames, wxT("ES")) )
		ctx.SegEs = val;
	else if ( 0 == wxStricmp(RegNames, wxT("CS")) )
		ctx.SegCs = val;
	else if ( 0 == wxStricmp(RegNames, wxT("SS")) )
		ctx.SegSs = val;
	else if ( 0 == wxStricmp(RegNames, wxT("DS")) )
		ctx.SegDs = val;
	else if ( 0 == wxStricmp(RegNames, wxT("FS")) )
		ctx.SegFs = val;
	else if ( 0 == wxStricmp(RegNames, wxT("GS")) )
		ctx.SegGs = val;
}

void RegRichTextCtrl::OnMoveMouse( wxMouseEvent& event )
{
}

//////////////////////////////////////////////////////////////////////////
RegFrame::RegFrame( wxMDIParentFrame *parent, const wxString& title )
  : ChildFrame(parent, title)
{
	m_panel = new wxPanel(this);
	m_sizerFrame = new wxBoxSizer(wxVERTICAL);

	m_textCtrlBasic = new RegRichTextCtrl(m_panel);

	m_bookCtrl = new wxAuiNotebook(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_BOTTOM);
	//m_bookCtrl = new wxNotebook(m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_BOTTOM);
	{
		wxWindow *page = MakePageDR();
		m_bookCtrl->AddPage(page, wxT("DR"), false);
	}

	{
		wxWindow *page = MakePageFPU();
		m_bookCtrl->AddPage(page, wxT("FPU"), false);
	}

	{
		wxWindow *page = MakePageMMX();
		m_bookCtrl->AddPage(page, wxT("MMX"), false);
	}

	{
		wxWindow *page = MakePage3dnow();
		m_bookCtrl->AddPage(page, wxT("3DNow!"), false);
	}

	//m_sizerFrame->Insert(0, m_bookCtrl, wxSizerFlags(5).Expand().Border());
	m_sizerFrame->Add(m_textCtrlBasic, 2, wxEXPAND);
	m_sizerFrame->Add(m_bookCtrl, 1, wxEXPAND);
	m_sizerFrame->Layout();
	m_panel->SetSizer(m_sizerFrame);

	LoadParams();
}

//////////////////////////////////////////////////////////////////////////
RegFrame::~RegFrame()
{
	SaveParams();

	delete m_textCtrlBasic;
	delete m_textCtrlDR;
	delete m_textCtrlFPU;
	delete m_textCtrlMMX;
	delete m_textCtrl3dnow;
	delete m_bookCtrl;
	delete m_panel;
}

//////////////////////////////////////////////////////////////////////////
// output values MMX registers
// TODO: make correct output view for this registers
void RegFrame::OutputMMX( const Context &ctx )
{
}

//////////////////////////////////////////////////////////////////////////
// output values 3dnow! registers
// TODO: make correct output view for this registers
void RegFrame::Output3dnow( const Context &ctx )
{
}

//////////////////////////////////////////////////////////////////////////
// output values FPU registers
// TODO: make correct output view for this registers
void RegFrame::OutputFPU( const Context &ctx )
{
}

//////////////////////////////////////////////////////////////////////////
void RegFrame::Output( TrcDebugEvent& event )
{
	Context ctx;
	m_sesId = event.getSesId();
	m_tid = (ULONG)event.getDebugEvent().CurrentThread->TID;

	m_textCtrlBasic->SetContextId(m_sesId);
	m_textCtrlBasic->SetThreadId(m_tid);

	trc_get_thread_ctx(m_sesId, m_tid, &ctx);

	m_textCtrlBasic->OutputBasic(ctx);
	m_textCtrlDR->OutputDR(ctx);
}

//////////////////////////////////////////////////////////////////////////
void RegFrame::SaveParams()
{
	this->SaveWinCoord(wxT("ChildFrame/Reg"));
}

//////////////////////////////////////////////////////////////////////////
void RegFrame::LoadParams()
{
	this->LoadWinCoord(wxT("ChildFrame/Reg"));
}

//////////////////////////////////////////////////////////////////////////
wxPanel* RegFrame::MakePageDR()
{
	wxPanel *panel = new wxPanel(m_bookCtrl);
	wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
	
	m_textCtrlDR = new RegRichTextCtrl(panel);
	sizerPanel->Add(m_textCtrlDR, 1, wxEXPAND , 0);
	panel->SetSizer(sizerPanel);

	return panel;
}

//////////////////////////////////////////////////////////////////////////
wxPanel* RegFrame::MakePageMMX()
{
	wxPanel *panel = new wxPanel(m_bookCtrl);
	wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
	
	m_textCtrlMMX = new RegRichTextCtrl(panel);

	sizerPanel->Add(m_textCtrlMMX, 1, wxEXPAND , 0);
	panel->SetSizer(sizerPanel);

	return panel;
}

//////////////////////////////////////////////////////////////////////////
wxPanel* RegFrame::MakePageFPU()
{
	wxPanel *panel = new wxPanel(m_bookCtrl);
	wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
	
	m_textCtrlFPU = new RegRichTextCtrl(panel);

	sizerPanel->Add(m_textCtrlFPU, 1, wxEXPAND , 0);
	panel->SetSizer(sizerPanel);

	return panel;
}

//////////////////////////////////////////////////////////////////////////
wxPanel* RegFrame::MakePage3dnow()
{
	wxPanel *panel = new wxPanel(m_bookCtrl);
	wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
	
	m_textCtrl3dnow = new RegRichTextCtrl(panel);

	sizerPanel->Add(m_textCtrl3dnow, 1, wxEXPAND , 0);
	panel->SetSizer(sizerPanel);

	return panel;
}

//////////////////////////////////////////////////////////////////////////
int RegFrame::GetContextId()
{
	return m_sesId;
}

void RegFrame::OnSize( wxSizeEvent& event )
{
	m_panel->SetClientSize(event.GetSize());
}