#ifndef __MEMCTRL_H__
#define __MEMCTRL_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "../precomp.h"

#include "column.h"

#include <wx/control.h>
#include <wx/caret.h>
#include <wx/dcclient.h>

class ListTextCtrl : public wxControl,
                     public wxScrollHelper
{
	DECLARE_DYNAMIC_CLASS(ListTextCtrl)
	DECLARE_EVENT_TABLE()

public:
	ListTextCtrl();
	ListTextCtrl( wxWindow *parent );
	~ListTextCtrl();

private:
	void Init();
	bool Create(wxWindow *parent, wxWindowID id,
	            const wxPoint& pos = wxDefaultPosition,
	            const wxSize& size = wxDefaultSize,
	            long style = 0, const wxValidator& validator = wxDefaultValidator,
	            const wxString& name = wxControlNameStr);

public:
	// useful functions
	void SetFontSize(int fontSize);
	void MoveCaret(int x, int y);
	void SetValue(const wxString &text);
	void SetDumpValue(const wxString &str);
	void SetAddrValue(unsigned startAddr, unsigned szAddr);
	void SetAlign(int align);

	void SetScrollCur(int scrollPos);
	int GetScrollCur();

	void SetHexSelection(const wxPoint& evtPos);
	void SetDumpSelection(const wxPoint& evtPos);

	void UpdateWindowSize();

public:
	// event handlers
	void OnPaint( wxPaintEvent& event );
	void OnEraseBackground(wxEraseEvent& event);
	void OnSize( wxSizeEvent& event );
	void OnChar( wxKeyEvent& event );
	void OnMouseMove( wxMouseEvent& event );
	void OnMouseLUp( wxMouseEvent& event );
	void OnMouseLDown( wxMouseEvent& event );
	void OnMouseRDown( wxMouseEvent& event );
	void OnMouseMDown( wxMouseEvent& event );
	void OnScroll(wxScrollWinEvent& event);

private:
	wxFont   m_font;

	int      m_xMargin, m_yMargin; // the margin around the text (looks nicer)
	long     m_fontW, m_fontH; // size (in pixels) of one character
	int      m_xChars, m_yChars; // the size (in text coords) of the window

	int      m_scrollLines;
	int      m_scrollCur;

	unsigned m_startAddr;
	unsigned m_szAddr;

private:
	Column m_addrCol;
	Column m_hexCol;
	Column m_dumpCol;

private:
	bool m_isAddrDown;
	bool m_isHexDown;
	bool m_isDumpDown;

	bool m_isHexSel;
	bool m_isDumpSel;

public:
	// scroll movement
	void SetScrollColumns(int scrollPos);
};

#endif // __MEMCTRL_H__
