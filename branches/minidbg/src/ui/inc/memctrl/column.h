/*! \file column.h
* \brief Header of column control

This control used for presenting data in style like ListView
* \author d1mk4
*/

#ifndef __COLUMN_H__
#define __COLUMN_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#define DEFAULT_COLUMN_WIDTH 10

#include "../precomp.h"

#include <wx/control.h>
#include <wx/dcclient.h>

class Column : public wxControl
{
public:
	typedef wxBufferedPaintDC DCType;

public:
	Column(int width = 0);
	Column(const wxString& name,
	       const wxFont& font,
	       int startX = 0,
	       int width = DEFAULT_COLUMN_WIDTH);

	void Init();
	void InitAddrCol();
	void Draw(DCType &dc);
	void DrawTextBuffer(DCType &dc);
	void DrawNextColumn(DCType &dc);
	void DrawSelection(DCType &dc);
	void DrawCaption(DCType &dc);

	void InitDrawing(DCType &dc);

	void SetWidth(int xChar);
	int GetWidth();

	void SetScrollCursor(int curPos);
	int GetScrollCursor();
	void SetScrollLines(int curLines);

	void SetStartPos(int pos);
	void SetEndPos(int pos);
	int GetStartPos();
	int GetEndPos();

	void SetViewStart(const wxPoint &pt);
	void SetText(const wxString &str);
	void SetCaretSize(int xChars, int yChars);

	void SetCaretPos( const std::vector<wxPoint> &selPoint ); // set graphical coordinates for caret
	std::vector<wxPoint> GetCaretPos(); // get graphical coordinates position of column caret

	void SetMousePos(const wxPoint &pt);
	wxPoint GetMousePos();

	void DrawAddr(DCType &dc, unsigned startAddr);

	void AddNextCol(Column &col);
	void AddPrevCol(Column &col);
	const Column& GetNextCol();
	const Column& GetPrevCol();

	void ClearSelection();
	void SetSelectionExt(int coefExt);

	wxPoint SelectHexPoint(int x, int y);
	void SelectHexLine(const wxPoint& startPt, int x, int y);
	void SelectHexLines(const wxPoint& startPt, int pos, int posEnd, int x, int y);

	wxPoint SelectDumpPoint(int x, int y);
	void SelectDumpLines(const wxPoint& startPt, int pos, int posEnd, int x, int y);

	wxPoint DeviceCoordsToGraphicalChars(const wxPoint& pos) const;
	wxPoint GraphicalCharToDeviceCoords(const wxPoint& pos) const;
	wxPoint LogicalCoordsToGraphicalChars(const wxPoint& pos) const;
	wxPoint GraphicalCharToLogicalCoords(const wxPoint& pos) const;

private:
	wxChar& CharAt(int x, int y);

public:
	wxFont m_font;
	wxClientDC m_dc;

private:
	int m_xStart; // start coordinate for drawing region
	int m_yStart;
	int m_xEnd; // end coordinate for drawing region
	int m_xDelta; // size between previous and next separator
	bool m_xChanged; // if column separator is move = true else = false 

	bool m_isAddrCol;
	unsigned m_startAddr;

	int m_xChars, m_yChars;
	int m_xMargin, m_yMargin;
	int m_fontW, m_fontH;
	int m_xCaretSz, m_yCaretSz;
	int m_xViewStart, m_yViewStart;

	int m_scrollCur;
	int m_scrollSz;

	int m_coefSelExt;

	wxSize m_szClient;
	wxPoint m_mousePt;

private:
	wxString m_text;

	std::vector<wxPoint> m_selPoint; //< for storing selected points 

private:
	wxString m_name;
	Column *m_nextCol;
	Column *m_prevCol;
};

#endif // __COLUMN_H__
