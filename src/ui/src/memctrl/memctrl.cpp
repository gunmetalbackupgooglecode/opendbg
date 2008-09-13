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

#include "memctrl/memctrl.h"

// TODO: обработку событий мышки при нажатии на символ и позиционирование каретки на этот символ
// TODO: организовать менеджер позиционировани€ по двум колонкам каретки и двух виртуальных кареток

#define COLUMN_LINE_DELTA 3

IMPLEMENT_DYNAMIC_CLASS(ListTextCtrl, wxControl)

BEGIN_EVENT_TABLE(ListTextCtrl, wxControl)
	EVT_ERASE_BACKGROUND(ListTextCtrl::OnEraseBackground)
	EVT_PAINT(ListTextCtrl::OnPaint)
	EVT_SIZE(ListTextCtrl::OnSize)
	EVT_CHAR(ListTextCtrl::OnChar)
	EVT_LEFT_DOWN(ListTextCtrl::OnMouseLDown)
	EVT_LEFT_UP(ListTextCtrl::OnMouseLUp)
	EVT_RIGHT_DOWN(ListTextCtrl::OnMouseRDown)
	EVT_MIDDLE_DOWN(ListTextCtrl::OnMouseMDown)
	EVT_MOTION(ListTextCtrl::OnMouseMove)
	EVT_SCROLLWIN(ListTextCtrl::OnScroll)
END_EVENT_TABLE()

const int g_fontSize = 10;

ListTextCtrl::ListTextCtrl( wxWindow *parent )
        : wxScrollHelper(this),
          m_addrCol( wxT("Address"), wxFont(g_fontSize, wxFONTFAMILY_TELETYPE,
                     wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL), 0, 10 ),
          m_hexCol( wxT("Hex dump"), wxFont(g_fontSize, wxFONTFAMILY_TELETYPE,
                    wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL), m_addrCol.GetEndPos(), 10),
          m_dumpCol( wxT("ASCII"), wxFont(g_fontSize, wxFONTFAMILY_TELETYPE,
                    wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL), m_hexCol.GetEndPos(), 10)
{
	Init();
	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL);
}

ListTextCtrl::ListTextCtrl()
      : wxScrollHelper(this),
          m_addrCol( wxT("Address"), wxFont(g_fontSize, wxFONTFAMILY_TELETYPE,
                     wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL), 0, 10 ),
          m_hexCol( wxT("Hex dump"), wxFont(g_fontSize, wxFONTFAMILY_TELETYPE,
                    wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL), m_addrCol.GetEndPos(), 10),
          m_dumpCol( wxT("ASCII"), wxFont(g_fontSize, wxFONTFAMILY_TELETYPE,
                    wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL), m_hexCol.GetEndPos(), 10)
{
	Init();
}

ListTextCtrl::~ListTextCtrl()
{
}

bool ListTextCtrl::Create( wxWindow *parent, wxWindowID id, const wxPoint& pos,
                           const wxSize& size, long style,
                           const wxValidator& validator, const wxString& name )
{
	if (!wxControl::Create(parent, id, pos, size, style, validator, name))
		return false;

	return true;
}

void ListTextCtrl::Init()
{
	m_isAddrDown = false;
	m_isHexDown = false;
	m_isDumpDown = false;
	m_isHexSel = false;
	m_isDumpSel = false;

	m_scrollCur = 0;

	m_addrCol.AddNextCol(m_hexCol);
	m_hexCol.AddNextCol(m_dumpCol);
	m_dumpCol.AddPrevCol(m_hexCol);
	m_hexCol.AddPrevCol(m_addrCol);

	m_hexCol.SetCaretSize(1, 1);

	SetBackgroundColour(*wxWHITE);

	SetFontSize(g_fontSize);
	m_dumpCol.SetWidth(16);

	m_scrollLines = 0;
	m_xChars = m_yChars = 0;
	m_xMargin = m_yMargin = 5;
}

void ListTextCtrl::SetFontSize(int fontSize)
{
	m_font = wxFont(fontSize, wxFONTFAMILY_TELETYPE,
	                wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	wxClientDC dc(this);
	dc.SetFont(m_font);
	m_fontH = dc.GetCharHeight();
	m_fontW = dc.GetCharWidth();
}

void ListTextCtrl::UpdateWindowSize()
{
	wxSize size = GetClientSize();
	m_xChars = (size.x - 2*m_xMargin) / m_fontW;
	m_yChars = (size.y - 2*m_yMargin) / m_fontH;
	if ( !m_xChars )
		m_xChars = 1;
	if ( !m_yChars )
		m_yChars = 1;
}

void ListTextCtrl::OnSize(wxSizeEvent& event)
{
	UpdateWindowSize();

	SetScrollbars(0, m_fontH, 0, m_scrollLines);

	event.Skip();
}

void ListTextCtrl::OnScroll( wxScrollWinEvent& event )
{
	// need to move the cursor when autoscrolling
	// FIXME: the cursor also moves when the scrollbar arrows are clicked
	if (event.GetOrientation() == wxVERTICAL)
	{
		if (event.GetEventType() == wxEVT_SCROLLWIN_THUMBTRACK)
		{
			m_scrollCur = event.GetPosition();
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_THUMBRELEASE)
		{
			m_scrollCur = event.GetPosition();
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEUP)
		{
			if (m_scrollCur >= 1)
				m_scrollCur-=1;
		}
		else if (event.GetEventType() == wxEVT_SCROLLWIN_LINEDOWN)
		{
			if (m_scrollCur <= m_scrollLines-m_yChars)
				m_scrollCur+=1;
		}
	}

	UpdateWindowSize();
	SetScrollColumns(m_scrollCur);
	Refresh();

	//event.Skip();
}

void ListTextCtrl::OnPaint( wxPaintEvent &event )
{
	wxBufferedPaintDC dc(this);
	PrepareDC( dc );
	dc.Clear();

	UpdateWindowSize();

	m_addrCol.Draw(dc);
	//m_hexCol.Draw(dc);
	//m_dumpCol.Draw(dc);

	event.Skip();
}

void ListTextCtrl::OnChar( wxKeyEvent &event )
{
	// TODO: !!!! заменить весь повтор€ющийс€ код на функции !!!!
	switch ( event.GetKeyCode() )
	{
		case WXK_PAGEUP:
			if (m_scrollCur >= 1)
			{
				--m_scrollCur;
				SetScrollColumns(m_scrollCur);
			}
			break;

		case WXK_PAGEDOWN:
			if (m_scrollCur <= m_scrollLines-m_yChars)
			{
				++m_scrollCur;
				SetScrollColumns(m_scrollCur);
			}
			break;

		case WXK_UP:
			if (m_scrollCur >= 1)
			{
				--m_scrollCur;
				SetScrollColumns(m_scrollCur);
			}
			break;

		case WXK_DOWN:
			if (m_scrollCur <= m_scrollLines-m_yChars)
			{
				++m_scrollCur;
				SetScrollColumns(m_scrollCur);
			}
			break;

		case WXK_HOME:
			m_scrollCur = 0;
			SetScrollColumns(m_scrollCur);
			break;

		case WXK_END:
			m_scrollCur = m_scrollLines-m_yChars;
			SetScrollColumns(m_scrollCur);
			break;

		default:
			event.Skip();
	}

	Refresh();
	event.Skip();
}

void ListTextCtrl::OnMouseMove( wxMouseEvent &event )
{
	wxPoint evtPos = event.GetPosition();
	int xAddrEndPos = m_addrCol.GetEndPos();
	int xHexEndPos = m_hexCol.GetEndPos();
	int xDmpEndPos = m_dumpCol.GetEndPos();

	wxSize szWin = GetSize();

	if ( (xAddrEndPos-COLUMN_LINE_DELTA <= evtPos.x && evtPos.x <= xAddrEndPos+COLUMN_LINE_DELTA) ||
	     (xHexEndPos-COLUMN_LINE_DELTA <= evtPos.x && evtPos.x <= xHexEndPos+COLUMN_LINE_DELTA) ||
	     (xDmpEndPos-COLUMN_LINE_DELTA <= evtPos.x && evtPos.x <= xDmpEndPos+COLUMN_LINE_DELTA) )
		this->SetCursor(wxCURSOR_SIZEWE);
	else
		this->SetCursor(wxCURSOR_ARROW);

	if ( event.LeftIsDown() )
	{
		if (m_isAddrDown)
		{
			if (szWin.GetWidth() > evtPos.x)
				m_addrCol.SetEndPos(evtPos.x);
			SetCursor(wxCURSOR_SIZEWE);
			Refresh();
		}
		else if (m_isHexDown)
		{
			if (szWin.GetWidth() > evtPos.x)
				m_hexCol.SetEndPos(evtPos.x);
			SetCursor(wxCURSOR_SIZEWE);
			Refresh();
		}
		else if (m_isDumpDown)
		{
			if (szWin.GetWidth() > evtPos.x)
				m_dumpCol.SetEndPos(evtPos.x);
			SetCursor(wxCURSOR_SIZEWE);
			Refresh();
		}

		if (m_isDumpSel)
		{
			SetHexSelection(evtPos);
			Refresh();
		}

		if (m_isHexSel)
		{
			SetDumpSelection(evtPos);
			Refresh();
		}
	}

	event.Skip();
}

void ListTextCtrl::SetValue( const wxString &text )
{
	//m_text = text;
}

void ListTextCtrl::OnMouseLDown( wxMouseEvent& event )
{
	m_hexCol.ClearSelection();
	m_dumpCol.ClearSelection();
	Refresh();

	wxPoint evtPos = event.GetPosition();

	int xAddrEndPos = m_addrCol.GetEndPos();
	int xHexEndPos = m_hexCol.GetEndPos();
	int xDmpEndPos = m_dumpCol.GetEndPos();

	if (xAddrEndPos-COLUMN_LINE_DELTA <= evtPos.x && evtPos.x <= xAddrEndPos+COLUMN_LINE_DELTA)
		m_isAddrDown = true;
	else if (xHexEndPos-COLUMN_LINE_DELTA <= evtPos.x && evtPos.x <= xHexEndPos+COLUMN_LINE_DELTA)
		m_isHexDown = true;
	else if (xDmpEndPos-COLUMN_LINE_DELTA <= evtPos.x && evtPos.x <= xDmpEndPos+COLUMN_LINE_DELTA)
		m_isDumpDown = true;

	if ( xHexEndPos < evtPos.x/* && evtPos.x <= xDmpEndPos*/ )
	{
		m_isDumpSel = true;
		SetHexSelection(evtPos);
		Refresh();
	}
	else if ( xAddrEndPos <= evtPos.x && evtPos.x <= xHexEndPos )
	{
		m_isHexSel = true;
		SetDumpSelection(evtPos);
		Refresh();
	}

	event.Skip();
}

void ListTextCtrl::OnMouseLUp( wxMouseEvent& event )
{
	m_isAddrDown = false;
	m_isHexDown = false;
	m_isDumpDown = false;

	m_isDumpSel = false;

	event.Skip();
}

void ListTextCtrl::OnMouseRDown( wxMouseEvent& event )
{
	m_hexCol.ClearSelection();
	m_dumpCol.ClearSelection();
	Refresh();

	event.Skip();
}

void ListTextCtrl::OnMouseMDown( wxMouseEvent& event )
{
	event.Skip();
}

void ListTextCtrl::OnEraseBackground( wxEraseEvent& event )
{
}

void ListTextCtrl::SetDumpValue( const wxString &str )
{
	wxString hexStr;
	m_dumpCol.SetText(str);

	for (size_t i = 0; i < str.size(); ++i)
	{
		hexStr.Append( wxString::Format(wxT("%.2X"), (wxUChar)str[i]) );
		hexStr.Append(wxString::Format(wxT(" ")));
	}

	m_hexCol.SetWidth(m_dumpCol.GetWidth()*2+16);

	m_hexCol.SetText(hexStr);
	m_dumpCol.SetStartPos(m_hexCol.GetEndPos());

	m_scrollLines = static_cast<int>(str.size())/m_dumpCol.GetWidth();

	SetScrollbar(wxVERTICAL, 0, m_fontH, m_scrollLines); // initialization for show vertical scrollbar
	SetScrollbars(0, m_fontH, 0, m_scrollLines);
	m_hexCol.SetScrollLines(m_scrollLines);
	m_dumpCol.SetScrollLines(m_scrollLines);

	Refresh();
}

void ListTextCtrl::SetAlign( int align )
{
}

void ListTextCtrl::SetHexSelection( const wxPoint& evtPos )
{
	m_dumpCol.SetMousePos(evtPos);
	std::vector<wxPoint> vecPt( m_dumpCol.GetCaretPos() );
	std::vector<wxPoint> hexPt;

	for (size_t i = 0; i < vecPt.size(); ++i)
	{
		vecPt[i].x *= 3;

		// TODO: сделать оптимальней
		hexPt.push_back(wxPoint(vecPt[i].x, vecPt[i].y));
		hexPt.push_back(wxPoint(vecPt[i].x+1, vecPt[i].y));
		hexPt.push_back(wxPoint(vecPt[i].x+2, vecPt[i].y));
	}

	m_hexCol.SetCaretPos(hexPt);
}

void ListTextCtrl::SetDumpSelection( const wxPoint& evtPos )
{
	m_hexCol.SetMousePos(evtPos);
	std::vector<wxPoint> vecPt( m_hexCol.GetCaretPos() );

	for (size_t i = 0; i < vecPt.size(); ++i)
		vecPt[i].x /= 3;
	
	m_dumpCol.SetCaretPos(vecPt);
}

void ListTextCtrl::SetAddrValue( unsigned startAddr, unsigned szAddr )
{
	m_startAddr = startAddr; // set first address 
	m_szAddr = szAddr;

	wxString addrStr;

	for (size_t i = 0; i < szAddr; i+=16)
		addrStr.Append(wxString::Format("%.8X", startAddr+i));

	m_addrCol.SetText(addrStr);
	m_addrCol.InitAddrCol();
}

void ListTextCtrl::SetScrollColumns(int scrollPos)
{
	SetScrollPos(wxVERTICAL, scrollPos);
	m_addrCol.SetScrollCursor(scrollPos);
	m_hexCol.SetScrollCursor(scrollPos);
	m_dumpCol.SetScrollCursor(scrollPos);
}
