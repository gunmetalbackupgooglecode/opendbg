/*! \file cpulistview.h
* \brief Header of CPU listview control

This control provides all functionality for work with code's dump,
break points and some other
* \author d1mk4
*/


#ifndef __CPULISTVIEW_H__
#define __CPULISTVIEW_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "events.h"

class CPUListCtrl: public wxListCtrl
{
public:
	CPUListCtrl(wxWindow *parent,
             const wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             long style)
      : wxListCtrl(parent, id, pos, size, style),
        m_attr(*wxBLUE, *wxLIGHT_GREY, wxNullFont)
	{
#ifdef __POCKETPC__
	EnableContextMenu();
#endif
	}

	virtual ~CPUListCtrl()
	{
	}

	// add one item to the listctrl in report mode
	void InsertItemInReportView(int i);

	void OnColClick(wxListEvent& event);
	void OnColRightClick(wxListEvent& event);
	void OnColBeginDrag(wxListEvent& event);
	void OnColDragging(wxListEvent& event);
	void OnColEndDrag(wxListEvent& event);
	void OnBeginDrag(wxListEvent& event);
	void OnBeginRDrag(wxListEvent& event);
	void OnBeginLabelEdit(wxListEvent& event);
	void OnEndLabelEdit(wxListEvent& event);
	void OnDeleteItem(wxListEvent& event);
	void OnDeleteAllItems(wxListEvent& event);
		void OnRightClick(wxListEvent& event);
	#if WXWIN_COMPATIBILITY_2_4
	void OnGetInfo(wxListEvent& event);
	void OnSetInfo(wxListEvent& event);
	#endif
	void OnSelected(wxListEvent& event);
	void OnDeselected(wxListEvent& event);
	void OnListKeyDown(wxListEvent& event);
	void OnActivated(wxListEvent& event);
	void OnFocused(wxListEvent& event);
	void OnCacheHint(wxListEvent& event);

	void OnChar(wxKeyEvent& event);

#if USE_CONTEXT_MENU
	void OnContextMenu(wxContextMenuEvent& event);
#endif

	void OnToggleBP(wxCommandEvent& event);

public:
	void ShowContextMenu(const wxPoint& pos);
	
	void SetColumnImage(int col, int image);

	void LogEvent(const wxListEvent& event, const wxChar *eventName);
	void LogColEvent(const wxListEvent& event, const wxChar *eventName);

	virtual wxString OnGetItemText(long item, long column) const;
	virtual int OnGetItemColumnImage(long item, long column) const;
	virtual wxListItemAttr *OnGetItemAttr(long item) const;

	wxListItemAttr m_attr;
	
private:
	wxString m_address;
	wxString m_hexdump;
	wxString m_disassembly;
	wxListItem m_lastItem;
	
private:
	DECLARE_NO_COPY_CLASS(CPUListCtrl)
	DECLARE_EVENT_TABLE()
};

	// IDs for the menu commands
	enum
	{
		CPU_LIST_CTRL                   = 20001,
		CPU_LIST_CTRL_TOGGLE_BP
	};

#endif // __CPULISTVIEW_H__
