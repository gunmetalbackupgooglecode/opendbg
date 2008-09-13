#ifndef __LISTVIEW_H__
#define __LISTVIEW_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

class ListCtrl: public wxListCtrl
{
public:
	ListCtrl(wxWindow *parent,
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

	virtual ~ListCtrl()
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

	void OnRightClick(wxMouseEvent& event);

public:
	void ShowContextMenu(const wxPoint& pos);
	void SetColumnImage(int col, int image);

	void LogEvent(const wxListEvent& event, const wxChar *eventName);
	void LogColEvent(const wxListEvent& event, const wxChar *eventName);

	virtual wxString OnGetItemText(long item, long column) const;
	virtual int OnGetItemColumnImage(long item, long column) const;
	virtual wxListItemAttr *OnGetItemAttr(long item) const;

	wxListItemAttr m_attr;

	DECLARE_NO_COPY_CLASS(ListCtrl)
	DECLARE_EVENT_TABLE()
	};

	// IDs for the menu commands
	enum
	{
	LIST_ABOUT = wxID_ABOUT,
	LIST_QUIT = wxID_EXIT,

	LIST_LIST_VIEW = wxID_HIGHEST,
	LIST_ICON_VIEW,
	LIST_ICON_TEXT_VIEW,
	LIST_SMALL_ICON_VIEW,
	LIST_SMALL_ICON_TEXT_VIEW,
	LIST_REPORT_VIEW,
	LIST_VIRTUAL_VIEW,
	LIST_SMALL_VIRTUAL_VIEW,

	LIST_DESELECT_ALL,
	LIST_SELECT_ALL,
	LIST_DELETE_ALL,
	LIST_DELETE,
	LIST_ADD,
	LIST_EDIT,
	LIST_SORT,
	LIST_SET_FG_COL,
	LIST_SET_BG_COL,
	LIST_TOGGLE_MULTI_SEL,
	LIST_TOGGLE_FIRST,
	LIST_SHOW_COL_INFO,
	LIST_SHOW_SEL_INFO,
	LIST_FOCUS_LAST,
	LIST_FREEZE,
	LIST_THAW,
	LIST_TOGGLE_LINES,
	LIST_MAC_USE_GENERIC,

	LIST_CTRL                   = 1000
};

#endif // __LISTVIEW_H__