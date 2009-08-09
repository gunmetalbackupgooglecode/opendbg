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

#include "listview.h"

BEGIN_EVENT_TABLE(ListCtrl, wxListCtrl)
    EVT_LIST_BEGIN_DRAG(LIST_CTRL, ListCtrl::OnBeginDrag)
    EVT_LIST_BEGIN_RDRAG(LIST_CTRL, ListCtrl::OnBeginRDrag)
    EVT_LIST_BEGIN_LABEL_EDIT(LIST_CTRL, ListCtrl::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(LIST_CTRL, ListCtrl::OnEndLabelEdit)
    EVT_LIST_DELETE_ITEM(LIST_CTRL, ListCtrl::OnDeleteItem)
    EVT_LIST_DELETE_ALL_ITEMS(LIST_CTRL, ListCtrl::OnDeleteAllItems)
#if WXWIN_COMPATIBILITY_2_4
    EVT_LIST_GET_INFO(LIST_CTRL, ListCtrl::OnGetInfo)
    EVT_LIST_SET_INFO(LIST_CTRL, ListCtrl::OnSetInfo)
#endif
    EVT_LIST_ITEM_SELECTED(LIST_CTRL, ListCtrl::OnSelected)
    EVT_LIST_ITEM_DESELECTED(LIST_CTRL, ListCtrl::OnDeselected)
    //EVT_LIST_ITEM_RIGHT_CLICK(LIST_CTRL, ListCtrl::)
    EVT_LIST_KEY_DOWN(LIST_CTRL, ListCtrl::OnListKeyDown)
    EVT_LIST_ITEM_ACTIVATED(LIST_CTRL, ListCtrl::OnActivated)
    EVT_LIST_ITEM_FOCUSED(LIST_CTRL, ListCtrl::OnFocused)

    EVT_LIST_COL_CLICK(LIST_CTRL, ListCtrl::OnColClick)
    EVT_LIST_COL_RIGHT_CLICK(LIST_CTRL, ListCtrl::OnColRightClick)
    EVT_LIST_COL_BEGIN_DRAG(LIST_CTRL, ListCtrl::OnColBeginDrag)
    EVT_LIST_COL_DRAGGING(LIST_CTRL, ListCtrl::OnColDragging)
    EVT_LIST_COL_END_DRAG(LIST_CTRL, ListCtrl::OnColEndDrag)

    EVT_LIST_CACHE_HINT(LIST_CTRL, ListCtrl::OnCacheHint)

#if USE_CONTEXT_MENU
    EVT_CONTEXT_MENU(ListCtrl::OnContextMenu)
#endif
    EVT_CHAR(ListCtrl::OnChar)

    //EVT_RIGHT_DOWN(ListCtrl::OnRightClick)
END_EVENT_TABLE()

// ListCtrl

void ListCtrl::OnCacheHint(wxListEvent& event)
{
}

void ListCtrl::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);
    item.SetImage(image);
    SetColumn(col, item);
}

void ListCtrl::OnColClick(wxListEvent& event)
{
    //int col = event.GetColumn();

    //// set or unset image
    //static bool x = false;
    //x = !x;
    //SetColumnImage(col, x ? 0 : -1);

    //wxLogMessage( wxT("OnColumnClick at %d."), col );
}

void ListCtrl::OnColRightClick(wxListEvent& event)
{
    //int col = event.GetColumn();
    //if ( col != -1 )
    //{
    //    SetColumnImage(col, -1);
    //}

    //// Show popupmenu at position
    //wxMenu menu(wxT("Test"));
    //menu.Append(LIST_ABOUT, _T("&About"));
    //PopupMenu(&menu, event.GetPoint());

    //wxLogMessage( wxT("OnColumnRightClick at %d."), event.GetColumn() );
}

void ListCtrl::LogColEvent(const wxListEvent& event, const wxChar *name)
{
    //const int col = event.GetColumn();

    //wxLogMessage(wxT("%s: column %d (width = %d or %d)."),
    //             name,
    //             col,
    //             event.GetItem().GetWidth(),
    //             GetColumnWidth(col));
}

void ListCtrl::OnColBeginDrag(wxListEvent& event)
{
    //LogColEvent( event, wxT("OnColBeginDrag") );

    //if ( event.GetColumn() == 0 )
    //{
    //    wxLogMessage(_T("Resizing this column shouldn't work."));

    //    event.Veto();
    //}
}

void ListCtrl::OnColDragging(wxListEvent& event)
{
//    LogColEvent( event, wxT("OnColDragging") );
}

void ListCtrl::OnColEndDrag(wxListEvent& event)
{
//    LogColEvent( event, wxT("OnColEndDrag") );
}

void ListCtrl::OnBeginDrag(wxListEvent& event)
{
    //const wxPoint& pt = event.m_pointDrag;

    //int flags;
    //wxLogMessage( wxT("OnBeginDrag at (%d, %d), item %ld."),
    //              pt.x, pt.y, HitTest(pt, flags) );
}

void ListCtrl::OnBeginRDrag(wxListEvent& event)
{
    //wxLogMessage( wxT("OnBeginRDrag at %d,%d."),
    //              event.m_pointDrag.x, event.m_pointDrag.y );
}

void ListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
//    wxLogMessage( wxT("OnBeginLabelEdit: %s"), event.m_item.m_text.c_str());
}

void ListCtrl::OnEndLabelEdit(wxListEvent& event)
{
    //wxLogMessage( wxT("OnEndLabelEdit: %s"),
    //              event.IsEditCancelled() ? _T("[cancelled]")
    //                                      : event.m_item.m_text.c_str());
}

void ListCtrl::OnDeleteItem(wxListEvent& event)
{
    //LogEvent(event, _T("OnDeleteItem"));
    //wxLogMessage( wxT("Number of items when delete event is sent: %d"), GetItemCount() );
}

void ListCtrl::OnDeleteAllItems(wxListEvent& event)
{
    //LogEvent(event, _T("OnDeleteAllItems"));
}

#if WXWIN_COMPATIBILITY_2_4
void ListCtrl::OnGetInfo(wxListEvent& event)
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

void ListCtrl::OnSetInfo(wxListEvent& event)
{
    LogEvent(event, _T("OnSetInfo"));
}
#endif

void ListCtrl::OnSelected(wxListEvent& event)
{

    if ( GetWindowStyle() & wxLC_REPORT )
    {
        wxListItem info;
        info.m_itemId = event.m_itemIndex;
        info.m_col = 0;
        info.m_mask = wxLIST_MASK_TEXT;
        GetItem(info);
    }
}

void ListCtrl::OnDeselected(wxListEvent& event)
{
    //LogEvent(event, _T("OnDeselected"));
}

void ListCtrl::OnActivated(wxListEvent& event)
{
    //LogEvent(event, _T("OnActivated"));
}

void ListCtrl::OnFocused(wxListEvent& event)
{
    //LogEvent(event, _T("OnFocused"));

   // event.Skip();
}

void ListCtrl::OnListKeyDown(wxListEvent& event)
{
    //long item;

    //switch ( event.GetKeyCode() )
    //{
    //    case 'c': // colorize
    //    case 'C':
    //        {
    //            wxListItem info;
    //            info.m_itemId = event.GetIndex();
    //            if ( info.m_itemId == -1 )
    //            {
    //                // no item
    //                break;
    //            }

    //            GetItem(info);

    //            wxListItemAttr *attr = info.GetAttributes();
    //            if ( !attr || !attr->HasTextColour() )
    //            {
    //                info.SetTextColour(*wxCYAN);

    //                SetItem(info);

    //                RefreshItem(info.m_itemId);
    //            }
    //        }
    //        break;

    //    case 'n': // next
    //    case 'N':
    //        item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
    //        if ( item++ == GetItemCount() - 1 )
    //        {
    //            item = 0;
    //        }

    //        wxLogMessage(_T("Focusing item %ld"), item);

    //        SetItemState(item, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
    //        EnsureVisible(item);
    //        break;

    //    case 'r': // show bounding Rect
    //    case 'R':
    //        {
    //            item = event.GetIndex();
    //            wxRect r;
    //            if ( !GetItemRect(item, r) )
    //            {
    //                wxLogError(_T("Failed to retrieve rect of item %ld"), item);
    //                break;
    //            }

    //            wxLogMessage(_T("Bounding rect of item %ld is (%d, %d)-(%d, %d)"),
    //                         item, r.x, r.y, r.x + r.width, r.y + r.height);
    //        }
    //        break;

    //    case WXK_DELETE:
    //        item = GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    //        while ( item != -1 )
    //        {
    //            DeleteItem(item);

    //            wxLogMessage(_T("Item %ld deleted"), item);

    //            // -1 because the indices were shifted by DeleteItem()
    //            item = GetNextItem(item - 1,
    //                               wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    //        }
    //        break;

    //    case WXK_INSERT:
    //        if ( GetWindowStyle() & wxLC_REPORT )
    //        {
    //            if ( GetWindowStyle() & wxLC_VIRTUAL )
    //            {
    //                SetItemCount(GetItemCount() + 1);
    //            }
    //            else // !virtual
    //            {
    //                InsertItemInReportView(event.GetIndex());
    //            }
    //        }
    //        //else: fall through

    //    default:
    //        LogEvent(event, _T("OnListKeyDown"));

    //        event.Skip();
    //}
}

void ListCtrl::OnChar(wxKeyEvent& event)
{
    //wxLogMessage(_T("Got char event."));

    //switch ( event.GetKeyCode() )
    //{
    //    case 'n':
    //    case 'N':
    //    case 'c':
    //    case 'C':
    //        // these are the keys we process ourselves
    //        break;

    //    default:
    //        event.Skip();
    //}
}

void ListCtrl::OnRightClick(wxMouseEvent& event)
{
    //if ( !event.ControlDown() )
    //{
    //    event.Skip();
    //    return;
    //}

    //int flags;
    //long subitem;
    //long item = HitTest(event.GetPosition(), flags, &subitem);

    //wxString where;
    //switch ( flags )
    //{
    //    case wxLIST_HITTEST_ABOVE: where = _T("above"); break;
    //    case wxLIST_HITTEST_BELOW: where = _T("below"); break;
    //    case wxLIST_HITTEST_NOWHERE: where = _T("nowhere near"); break;
    //    case wxLIST_HITTEST_ONITEMICON: where = _T("on icon of"); break;
    //    case wxLIST_HITTEST_ONITEMLABEL: where = _T("on label of"); break;
    //    case wxLIST_HITTEST_ONITEMRIGHT: where = _T("right on"); break;
    //    case wxLIST_HITTEST_TOLEFT: where = _T("to the left of"); break;
    //    case wxLIST_HITTEST_TORIGHT: where = _T("to the right of"); break;
    //    default: where = _T("not clear exactly where on"); break;
    //}

    //wxLogMessage(_T("Right double click %s item %ld, subitem %ld"),
    //             where.c_str(), item, subitem);
}

void ListCtrl::LogEvent(const wxListEvent& event, const wxChar *eventName)
{
}

wxString ListCtrl::OnGetItemText(long item, long column) const
{
	return wxString::Format(_T("Column %ld of item %ld"), column, item);
}

int ListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if (!column)
        return 0;

    if (!(item %3) && column == 1)
        return 0;

    return -1;
}

wxListItemAttr *ListCtrl::OnGetItemAttr(long item) const
{
    return item % 2 ? NULL : (wxListItemAttr *)&m_attr;
}

void ListCtrl::InsertItemInReportView(int i)
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
void ListCtrl::OnContextMenu(wxContextMenuEvent& event)
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

void ListCtrl::ShowContextMenu(const wxPoint& pos)
{
    wxMenu menu;

    menu.Append(wxID_ABOUT, _T("&About"));
    menu.AppendSeparator();
    menu.Append(wxID_EXIT, _T("E&xit"));

    PopupMenu(&menu, pos.x, pos.y);
}
