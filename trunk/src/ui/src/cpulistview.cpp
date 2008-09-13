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

#include "cpulistview.h"


BEGIN_EVENT_TABLE(CPUListCtrl, wxListCtrl)
	EVT_LIST_BEGIN_DRAG(CPU_LIST_CTRL, CPUListCtrl::OnBeginDrag)
	EVT_LIST_BEGIN_RDRAG(CPU_LIST_CTRL, CPUListCtrl::OnBeginRDrag)
	EVT_LIST_BEGIN_LABEL_EDIT(CPU_LIST_CTRL, CPUListCtrl::OnBeginLabelEdit)
	EVT_LIST_END_LABEL_EDIT(CPU_LIST_CTRL, CPUListCtrl::OnEndLabelEdit)
	EVT_LIST_DELETE_ITEM(CPU_LIST_CTRL, CPUListCtrl::OnDeleteItem)
	EVT_LIST_DELETE_ALL_ITEMS(CPU_LIST_CTRL, CPUListCtrl::OnDeleteAllItems)
	#if WXWIN_COMPATIBILITY_2_4
	EVT_LIST_GET_INFO(CPU_LIST_CTRL, CPUListCtrl::OnGetInfo)
	EVT_LIST_SET_INFO(CPU_LIST_CTRL, CPUListCtrl::OnSetInfo)
	#endif
	EVT_LIST_ITEM_SELECTED(CPU_LIST_CTRL, CPUListCtrl::OnSelected)
	EVT_LIST_ITEM_DESELECTED(CPU_LIST_CTRL, CPUListCtrl::OnDeselected)
	EVT_LIST_ITEM_RIGHT_CLICK(CPU_LIST_CTRL, CPUListCtrl::OnRightClick)
	EVT_LIST_KEY_DOWN(CPU_LIST_CTRL, CPUListCtrl::OnListKeyDown)
	EVT_LIST_ITEM_ACTIVATED(CPU_LIST_CTRL, CPUListCtrl::OnActivated)
	EVT_LIST_ITEM_FOCUSED(CPU_LIST_CTRL, CPUListCtrl::OnFocused)

	EVT_LIST_COL_CLICK(CPU_LIST_CTRL, CPUListCtrl::OnColClick)
	EVT_LIST_COL_RIGHT_CLICK(CPU_LIST_CTRL, CPUListCtrl::OnColRightClick)
	EVT_LIST_COL_BEGIN_DRAG(CPU_LIST_CTRL, CPUListCtrl::OnColBeginDrag)
	EVT_LIST_COL_DRAGGING(CPU_LIST_CTRL, CPUListCtrl::OnColDragging)
	EVT_LIST_COL_END_DRAG(CPU_LIST_CTRL, CPUListCtrl::OnColEndDrag)

	EVT_LIST_CACHE_HINT(CPU_LIST_CTRL, CPUListCtrl::OnCacheHint)

	#if USE_CONTEXT_MENU
	EVT_CONTEXT_MENU(CPUListCtrl::OnContextMenu)
	#endif
	EVT_CHAR(CPUListCtrl::OnChar)

	EVT_MENU(CPU_LIST_CTRL_TOGGLE_BP, CPUListCtrl::OnToggleBP)

END_EVENT_TABLE()

// ListCtrl

void CPUListCtrl::OnCacheHint(wxListEvent& event)
{
    wxLogMessage( wxT("OnCacheHint: cache items %ld..%ld"),
                  event.GetCacheFrom(), event.GetCacheTo() );
}

void CPUListCtrl::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
}

void CPUListCtrl::OnColClick(wxListEvent& event)
{
}

void CPUListCtrl::OnColRightClick(wxListEvent& event)
{
}

void CPUListCtrl::LogColEvent(const wxListEvent& event, const wxChar *name)
{
}

void CPUListCtrl::OnColBeginDrag(wxListEvent& event)
{
}

void CPUListCtrl::OnColDragging(wxListEvent& event)
{
}

void CPUListCtrl::OnColEndDrag(wxListEvent& event)
{
}

void CPUListCtrl::OnBeginDrag(wxListEvent& event)
{
}

void CPUListCtrl::OnBeginRDrag(wxListEvent& event)
{
}

void CPUListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
}

void CPUListCtrl::OnEndLabelEdit(wxListEvent& event)
{
}

void CPUListCtrl::OnDeleteItem(wxListEvent& event)
{
}

void CPUListCtrl::OnDeleteAllItems(wxListEvent& event)
{
}

#if WXWIN_COMPATIBILITY_2_4
void CPUListCtrl::OnGetInfo(wxListEvent& event)
{
    wxString msg;

    msg << _T("OnGetInfo (") << event.m_item.m_itemId << _T(", ") << event.m_item.m_col << _T(")");
    if ( event.m_item.m_mask & wxLIST_MASK_STATE )
        msg << _T(" wxLIST_MASK_STATE");
    if ( event.m_item.m_mask & wxLIST_MASK_TEXT )
        msg << _T(" wxLIST_MASK_TEXT");
    if ( event.m_item.m_mask & wxLIST_MASK_IMAGE )
        msg << _T(" wxLIST_MASK_IMAGE");
    if ( event.m_item.m_mask & wxLIST_MASK_DATA )
        msg << _T(" wxLIST_MASK_DATA");
    if ( event.m_item.m_mask & wxLIST_SET_ITEM )
        msg << _T(" wxLIST_SET_ITEM");
    if ( event.m_item.m_mask & wxLIST_MASK_WIDTH )
        msg << _T(" wxLIST_MASK_WIDTH");
    if ( event.m_item.m_mask & wxLIST_MASK_FORMAT )
        msg << _T(" wxLIST_MASK_WIDTH");

    if ( event.m_item.m_mask & wxLIST_MASK_TEXT )
    {
        event.m_item.m_text = _T("My callback text");
    }

    wxLogMessage(msg);
}

void CPUListCtrl::OnSetInfo(wxListEvent& event)
{
}
#endif

void CPUListCtrl::OnSelected(wxListEvent& event)
{
	//if ( GetWindowStyle() & wxLC_REPORT )
	//{
	//	wxListItem info;
	//	info.m_itemId = event.m_itemIndex;
	//	info.m_col = 0;
	//	info.m_mask = wxLIST_MASK_TEXT;
	//	GetItem(info);

	//	GuiProcEvent evt(wxEVT_GUI_TEXT2WINDOW, GUI_MSG_DEL_BP);

	//	wxPostEvent(this->GetParent()->GetParent()->GetEventHandler(), evt);
	//}
}

void CPUListCtrl::OnDeselected(wxListEvent& event)
{
}

void CPUListCtrl::OnActivated(wxListEvent& event)
{
}

void CPUListCtrl::OnFocused(wxListEvent& event)
{
}

void CPUListCtrl::OnListKeyDown(wxListEvent& event)
{
}

void CPUListCtrl::OnChar(wxKeyEvent& event)
{
}

void CPUListCtrl::OnRightClick( wxListEvent& event )
{
	if ( GetWindowStyle() & wxLC_REPORT )
	{
		m_lastItem.m_itemId = event.m_itemIndex;
		m_lastItem.m_mask = wxLIST_MASK_TEXT;

		m_lastItem.m_col = 0;
		GetItem(m_lastItem);
		m_address = m_lastItem.m_text;

		m_lastItem.m_col = 1;
		GetItem(m_lastItem);
		m_hexdump = m_lastItem.m_text;

		m_lastItem.m_col = 2;
		GetItem(m_lastItem);
		m_disassembly = m_lastItem.m_text;
	}

	ShowContextMenu(event.GetPoint());
}

void CPUListCtrl::LogEvent(const wxListEvent& event, const wxChar *eventName)
{
    wxLogMessage(_T("Item %ld: %s (item text = %s, data = %ld)"),
                 event.GetIndex(), eventName,
                 event.GetText().c_str(), event.GetData());
}

wxString CPUListCtrl::OnGetItemText(long item, long column) const
{
	return wxString::Format(_T("Column %ld of item %ld"), column, item);
}

int CPUListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if (!column)
        return 0;

    if (!(item %3) && column == 1)
        return 0;

    return -1;
}

wxListItemAttr *CPUListCtrl::OnGetItemAttr(long item) const
{
    return item % 2 ? NULL : (wxListItemAttr *)&m_attr;
}

void CPUListCtrl::InsertItemInReportView(int i)
{
    //wxString buf;
    //buf.Printf(_T("This is item %d"), i);
    //long tmp = InsertItem(i, buf, 0);
    //SetItemData(tmp, i);

    //buf.Printf(_T("Col 1, item %d"), i);
    //SetItem(tmp, 1, buf);

    //buf.Printf(_T("Item %d in column 2"), i);
    //SetItem(tmp, 2, buf);
}

#if USE_CONTEXT_MENU
void CPUListCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    wxPoint point = event.GetPosition();
    // If from keyboard
    if (point.x == -1 && point.y == -1) {
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    } else {
        point = ScreenToClient(point);
    }
    ShowContextMenu(point);
}
#endif

//////////////////////////////////////////////////////////////////////////
// 
void CPUListCtrl::ShowContextMenu(const wxPoint& pos)
{
	wxMenu menu;
	wxMenu *menu_bp = new wxMenu;

	menu_bp->Append(CPU_LIST_CTRL_TOGGLE_BP, wxT("Toggle\tF2"));

	menu.AppendSubMenu(menu_bp, wxT("&Breakpoint"));

	PopupMenu(&menu, pos.x, pos.y);
}

//////////////////////////////////////////////////////////////////////////
//
void CPUListCtrl::OnToggleBP( wxCommandEvent& event )
{
	//if (*wxRED != m_lastItem.GetBackgroundColour())
	{
		//m_lastItem.SetBackgroundColour(*wxRED);
		//this->SetItem(m_lastItem);
		GuiProcEvent evt(wxEVT_GUI_TEXT2WINDOW, GUI_MSG_ADD_BP);
		evt.setAddrBP(this->m_address);
		evt.setDisasmBP(this->m_disassembly);
		wxPostEvent(this->GetParent()->GetParent()->GetEventHandler(), evt);
	}
	//else
	//{
	//	m_lastItem.SetBackgroundColour(*wxWHITE);
	//	this->SetItem(m_lastItem);
	//	GuiProcEvent evt(wxEVT_GUI_TEXT2WINDOW, GUI_MSG_DEL_BP);
	//	evt.setAddrBP(this->m_address);
	//	evt.setDisasmBP(this->m_disassembly);
	//	wxPostEvent(this->GetParent()->GetParent()->GetEventHandler(), evt);
	//}
}
