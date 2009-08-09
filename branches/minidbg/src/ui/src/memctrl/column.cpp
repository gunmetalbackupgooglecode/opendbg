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

#include "memctrl/column.h"

//////////////////////////////////////////////////////////////////////////
//
Column::Column( int width /*= 0*/ )
 : m_dc(this),
   m_xChars(width),
   m_font(wxFont(10, wxFONTFAMILY_TELETYPE,
                     wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL)),
   m_nextCol(0), m_prevCol(0), m_xDelta(0), m_coefSelExt(0)
{
	Init();
}

//////////////////////////////////////////////////////////////////////////
//
Column::Column( const wxString& name,
                const wxFont &font,
                int startX /*= 0*/,
                int width )
 : m_dc(this),
   m_xChars(width),
   m_font(font),
   m_nextCol(0), m_prevCol(0),
   m_xDelta(0),
   m_scrollSz(0),
   m_coefSelExt(0), m_isAddrCol(false),
   m_xViewStart(0), m_yViewStart(0),
   m_xCaretSz(1), m_yCaretSz(1),
   m_xMargin(5), m_yMargin(5),
   m_scrollCur(0)
{
	// init start column position
	m_xStart = startX; // give position from painting for this column start
	m_yStart = 10;
	m_name = name; // give name for this column
	Init();
}

//////////////////////////////////////////////////////////////////////////
//
void Column::Init()
{
	m_dc.SetFont(m_font);
	m_fontH = m_dc.GetCharHeight();
	m_fontW = m_dc.GetCharWidth();

	// init end column position
	m_xEnd = m_fontW*m_xChars + 2*m_xMargin + m_xStart;
}

//////////////////////////////////////////////////////////////////////////
//
void Column::InitDrawing( DCType &dc )
{
	PrepareDC( dc );
	dc.SetFont( m_font );
	m_szClient = dc.GetSize();
	m_yChars = (m_szClient.y-m_yStart) / m_fontH;
}

//////////////////////////////////////////////////////////////////////////
//
void Column::DrawAddr(DCType &dc, unsigned startAddr)
{
	PrepareDC( dc );
	dc.SetFont( m_font );

	for ( int y = m_scrollCur-m_yMargin; y < m_yChars + m_scrollCur+m_yMargin; ++y )
	{
		dc.DrawText( wxString::Format(wxT("%d"), startAddr), m_xMargin + m_xStart, m_yMargin + m_fontH*y );
		startAddr += 16;
	}
}

//////////////////////////////////////////////////////////////////////////
//
void Column::Draw( DCType &dc )
{
	InitDrawing(dc);

	wxPoint pt = GraphicalCharToDeviceCoords(wxPoint(0, m_yChars + m_yMargin + m_scrollCur));

	dc.SetPen(*wxTRANSPARENT_PEN);

	// draw rectangle which contains column
	dc.DrawRectangle(m_xStart, m_yStart, m_szClient.GetWidth(), pt.y + m_yMargin);

	DrawSelection(dc);

	// settings for drawing
	dc.SetPen(*wxBLACK_PEN);
	dc.SetBrush(*(wxWHITE_BRUSH));
	
	DrawCaption(dc);

	// draw colum line
	dc.DrawLine(m_xStart, 0, m_xStart, m_szClient.y + pt.y + m_yMargin);
	dc.DrawLine(m_xEnd, 0, m_xEnd, m_szClient.y + pt.y + m_yMargin);

	DrawTextBuffer(dc);
	DrawNextColumn(dc);
}

void Column::DrawTextBuffer( DCType &dc )
{
	// if address column, we should draw 8 symbols
	int N_xChars;
	if (m_isAddrCol)
		N_xChars = 8;
	else
		N_xChars = m_xChars;

	// draw text buffer
	for ( int y = 0; y < m_yChars; ++y )
	{
		wxString line;

		for ( int x = 0; x < N_xChars; ++x )
		{
			size_t pos = x + N_xChars*(y+m_scrollCur);

			if (pos < m_text.size())
				line += m_text[pos];
			else
				line += ' ';
		}

		dc.DrawText( line, m_xMargin + m_xStart,
			m_yMargin + m_yStart + m_fontH*y );
	}
}

void Column::DrawSelection( DCType &dc )
{
	// set grey brush for drawing grey rectangle
	dc.SetBrush(*wxGREY_BRUSH); 

	// fol all selection text draw grey rectangles
	for (size_t i = 0; i < m_selPoint.size(); ++i)
	{
		wxPoint ptDev = GraphicalCharToDeviceCoords(m_selPoint[i]);
		
		// if selection was make in the hex column set selection symbol = coefSelExt*symbol
		if (m_coefSelExt)
			dc.DrawRectangle(wxPoint(m_xStart+ptDev.x+m_xMargin, ptDev.y + m_yStart + m_yMargin),
				wxSize(m_xCaretSz*m_fontW+m_fontW, m_yCaretSz*m_fontH));
		else
			dc.DrawRectangle(wxPoint(m_xStart+ptDev.x+m_xMargin, ptDev.y + m_yStart + m_yMargin),
				wxSize(m_xCaretSz*m_fontW, m_yCaretSz*m_fontH));
	}
}

void Column::DrawCaption( DCType &dc )
{
	dc.SetPen(*wxTRANSPARENT_PEN);
	//dc.SetBrush(*wxGREY_BRUSH);

	wxColor light_grey_color = wxColor(200, 200, 200);
	wxColor dark_grey_color = wxColor(110, 110, 110);

	dc.SetBrush(wxBrush(light_grey_color));

	// draw light grey caption background
	dc.DrawRectangle(wxPoint(m_xStart, 0),
		wxSize(m_xEnd-m_xStart, m_fontH));

	dc.SetPen(*wxWHITE_PEN);

	// draw up and left line in white color
	dc.DrawLine(wxPoint(m_xStart, 0), wxPoint(m_xEnd, 0));
	dc.DrawLine(wxPoint(m_xStart+1, 0), wxPoint(m_xStart+1, m_fontH));

	dc.SetPen(wxPen(dark_grey_color));

	// draw bottom and right line in dark grey color
	dc.DrawLine(wxPoint(m_xStart, m_fontH), wxPoint(m_xEnd, m_fontH));
	dc.DrawLine(wxPoint(m_xEnd-1, 0), wxPoint(m_xEnd-1, m_fontH));

	dc.SetBrush(*(wxWHITE_BRUSH));

	dc.DrawText(m_name, m_xStart+m_xMargin, 0);
}

void Column::DrawNextColumn( DCType &dc )
{
	// for drag column outline and move other columns with delta
	if (m_nextCol)
	{
		if (m_xChanged)
		{
			m_nextCol->SetStartPos(m_xEnd);
			m_nextCol->SetEndPos(m_nextCol->GetEndPos()+m_xDelta);

			m_xChanged = false;
		}
		m_nextCol->Draw(dc);
	}
}

int Column::GetStartPos()
{
	return m_xStart;
}

int Column::GetEndPos()
{
	return m_xEnd;
}

void Column::SetStartPos( int pos )
{
	m_xStart = pos;
	m_xEnd = m_xStart + m_xChars*m_fontW+2*m_xMargin; // 2 is the number of pixels for align line
}

void Column::SetEndPos( int pos )
{
	m_xDelta = pos - m_xEnd;
	m_xEnd = pos;
	m_xChanged = true;
}

void Column::SetText( const wxString &str )
{
	m_text.assign(str.begin(), str.end());
}

wxChar& Column::CharAt( int x, int y )
{
	return *(m_text.begin() + x + m_xChars * y);
}


void Column::AddPrevCol( Column &col )
{
	m_prevCol = &col;
}

void Column::AddNextCol( Column &col )
{
	m_nextCol = &col;
}

const Column& Column::GetNextCol()
{
	return *m_nextCol;
}

const Column& Column::GetPrevCol()
{
	return *m_prevCol;
}

void Column::SetScrollCursor( int curPos )
{
	m_scrollCur = curPos;
}

int Column::GetScrollCursor()
{
	return m_scrollCur;
}

wxPoint Column::DeviceCoordsToGraphicalChars(const wxPoint& pos) const
{
	wxPoint updPos(pos);
	updPos.x /= m_fontW;

	updPos.y /= m_fontH;
	updPos.y += m_scrollCur;

	return pos;
}

wxPoint Column::GraphicalCharToDeviceCoords(const wxPoint& pos) const
{
	wxPoint updPos(pos);
	updPos.x *= m_fontW;

	updPos.y -= m_scrollCur;
	updPos.y *= m_fontH;

	return updPos;
}

wxPoint Column::LogicalCoordsToGraphicalChars
        (const wxPoint &pos) const
{
	wxPoint updPos(pos);
	updPos.x /= m_fontW;
	updPos.y /= m_fontH;
	return updPos;
}

wxPoint Column::GraphicalCharToLogicalCoords
        (const wxPoint &pos) const
{
	wxPoint updPos(pos);
	updPos.x *= m_fontW;
	updPos.y *= m_fontH;
	return pos;
}

void Column::SetCaretPos( const std::vector<wxPoint> &selPoint )
{
	m_selPoint.assign(selPoint.begin(), selPoint.end());
}

std::vector<wxPoint> Column::GetCaretPos()
{
	return m_selPoint;
}

void Column::SetWidth( int xChar )
{
	m_xChars = xChar;
	m_xEnd = m_xStart + GraphicalCharToDeviceCoords(wxPoint(m_xChars, m_yChars)).x + 2*m_xMargin;
	//m_xEnd = m_xStart + 1*m_xMargin*m_xChars;
}

int Column::GetWidth()
{
	return m_xChars;
}

void Column::SetScrollLines( int curLines )
{
	m_scrollSz = curLines;
}

void Column::SetMousePos( const wxPoint &pt )
{
	for ( int x = 0; x < m_xChars; ++x )
	{
		for ( int y = 0; y <= m_yChars; ++y )
		{
			wxPoint devpt = wxPoint(x*m_fontW, y*m_fontH);
			
			int ptX = devpt.x+m_xStart+m_xMargin;
			int ptY = devpt.y+m_yMargin;

			if (ptY <= pt.y && pt.y <= (ptY + m_fontH))
			{
				if (ptX <= pt.x && pt.x <= (ptX + m_fontW))
				{
					wxPoint startPt;
					int posEnd, pos;

					if (wxT("Hex dump") == m_name)
					{
						startPt = SelectHexPoint(x, y+m_scrollCur);
						SelectHexLine(startPt, x, y);

						posEnd = x + m_xChars*(y + m_scrollCur);
						pos = startPt.x + m_xChars*startPt.y;

						SelectHexLines(startPt, pos, posEnd, x, y);
					}
					else
					{
						startPt = SelectDumpPoint(x, y+m_scrollCur);

						posEnd = x + m_xChars*(y + m_scrollCur);
						pos = startPt.x + m_xChars*startPt.y;

						SelectDumpLines(startPt, pos, posEnd, x, y);
					}

					break;
				}
			}
		}
	}
}

wxPoint Column::GetMousePos()
{
	return m_mousePt;
}

void Column::SetCaretSize( int xChars, int yChars )
{
	m_xCaretSz = xChars;
	m_yCaretSz = yChars;
}

void Column::ClearSelection()
{
	m_selPoint.resize(0);
}

void Column::SetSelectionExt( int coefExt )
{
	m_coefSelExt = coefExt;
}

void Column::InitAddrCol()
{
	m_isAddrCol = true;
}

wxPoint Column::SelectHexPoint( int x, int y )
{
	if (wxT(' ') != CharAt(x, y+m_scrollCur) &&
		  wxT(' ') != CharAt(x+1, y+m_scrollCur))
	{
		m_selPoint.push_back(wxPoint(x, y+m_scrollCur));
		m_selPoint.push_back(wxPoint(x+1, y+m_scrollCur));
		return m_selPoint.front();
	}
	else if (wxT(' ') != CharAt(x, y+m_scrollCur) &&
		       wxT(' ') == CharAt(x+1, y+m_scrollCur))
	{
		m_selPoint.push_back(wxPoint(x, y+m_scrollCur));
		m_selPoint.push_back(wxPoint(x-1, y+m_scrollCur));
		return m_selPoint.front();
	}
	else
	{
		m_selPoint.push_back(wxPoint(x, y+m_scrollCur));
		return m_selPoint.front();
	}
}

void Column::SelectHexLine( const wxPoint& startPt, int x, int y )
{
	if (startPt.x < x)
	{
		if (wxT(' ') != CharAt(x, y+m_scrollCur))
		{
			m_selPoint.push_back(wxPoint(x, y+m_scrollCur));
			m_selPoint.push_back(wxPoint(x+1, y+m_scrollCur));
		}
		else
		{
			m_selPoint.push_back(wxPoint(x, y+m_scrollCur));
			m_selPoint.push_back(wxPoint(x+1, y+m_scrollCur));
			m_selPoint.push_back(wxPoint(x+2, y+m_scrollCur));
		}
	}
	else if (startPt.x > x)
	{
		if (wxT(' ') != CharAt(x, y+m_scrollCur))
		{
			m_selPoint.push_back(wxPoint(x, y+m_scrollCur));
			m_selPoint.push_back(wxPoint(x-1, y+m_scrollCur));
		}
		else
		{
			m_selPoint.push_back(wxPoint(x, y+m_scrollCur));
			m_selPoint.push_back(wxPoint(x-1, y+m_scrollCur));
			m_selPoint.push_back(wxPoint(x-2, y+m_scrollCur));
		}
	}
}

void Column::SelectHexLines( const wxPoint& startPt, int pos, int posEnd, int x, int y )
{
	int xSel = startPt.x, ySel = 0;

	if (startPt.y < y + m_scrollCur)
	{
		m_selPoint.resize(0);
		while (pos <= posEnd)
		{
			ySel = (pos - xSel)/m_xChars;
			if (wxT(' ') != CharAt(xSel, ySel))
			{
				//m_selPoint.push_back(wxPoint(xSel-1, ySel));
				m_selPoint.push_back(wxPoint(xSel, ySel));
				m_selPoint.push_back(wxPoint(xSel+1, ySel));
			}
			else
			{
				m_selPoint.push_back(wxPoint(xSel, ySel));
				m_selPoint.push_back(wxPoint(xSel+1, ySel));
				m_selPoint.push_back(wxPoint(xSel+2, ySel));
			}

			xSel += 1;
			xSel %= m_xChars;
			++pos;
		}
	}
	else if (startPt.y > y + m_scrollCur)
	{
		m_selPoint.resize(0);
		while (pos >= posEnd)
		{
			ySel = (pos - xSel)/m_xChars;
			if (wxT(' ') != CharAt(xSel, ySel))
			{
				m_selPoint.push_back(wxPoint(xSel+1, ySel));
				m_selPoint.push_back(wxPoint(xSel, ySel));
				m_selPoint.push_back(wxPoint(xSel-1, ySel));
			}
			else
			{
				m_selPoint.push_back(wxPoint(xSel, ySel));
				m_selPoint.push_back(wxPoint(xSel-1, ySel));
				m_selPoint.push_back(wxPoint(xSel-2, ySel));
			}

			if (!xSel)
				xSel = m_xChars;
			xSel -= 1;
			--pos;
		}
	}
}

wxPoint Column::SelectDumpPoint( int x, int y )
{
	m_selPoint.push_back(wxPoint(x, y+m_scrollCur));
	return m_selPoint.front();
}

void Column::SelectDumpLines( const wxPoint& startPt, int pos, int posEnd, int x, int y )
{
	int xSel = startPt.x, ySel = 0;

	if (startPt.y < y + m_scrollCur)
	{
		m_selPoint.resize(0);
		while (pos <= posEnd)
		{
			ySel = (pos - xSel)/m_xChars;
			m_selPoint.push_back(wxPoint(xSel, ySel));
			xSel += 1;
			xSel %= m_xChars;
			++pos;
		}
	}
	else if (startPt.y > y + m_scrollCur)
	{
		m_selPoint.resize(0);
		while (pos >= posEnd)
		{
			ySel = (pos - xSel)/m_xChars;
			m_selPoint.push_back(wxPoint(xSel, ySel));
			if (!xSel)
				xSel = m_xChars;
			--xSel;
			--pos;
		}
	}
}
