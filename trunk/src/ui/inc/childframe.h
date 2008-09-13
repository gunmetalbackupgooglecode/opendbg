#ifndef __CHILDFRAME_H__
#define __CHILDFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "listview.h"

class ChildFrame : public wxControl
{
public:
	ChildFrame(wxMDIParentFrame *parent, const wxString& title);
	virtual ~ChildFrame();

	void OnActivate(wxActivateEvent& event);

	void OnRefresh(wxCommandEvent& event);
	void OnUpdateRefresh(wxUpdateUIEvent& event);
	void OnChangeTitle(wxCommandEvent& event);
	void OnChangePosition(wxCommandEvent& event);
	void OnChangeSize(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMove(wxMoveEvent& event);
	void OnClose(wxCloseEvent& event);

	// each window must have it's own store procedure
	virtual void SaveParams() = 0;
	virtual void LoadParams() = 0;

	virtual void SaveWinCoord(const wxString &path);
	virtual void LoadWinCoord(const wxString &path);

	#if wxUSE_CLIPBOARD
	void OnPaste(wxCommandEvent& event);
	void OnUpdatePaste(wxUpdateUIEvent& event);
	#endif // wxUSE_CLIPBOARD

	DECLARE_EVENT_TABLE()
};

// menu items ids
enum
{
	MDI_QUIT = wxID_EXIT,
	MDI_NEW_WINDOW = 101,

	MDI_VIEW_CPU,
	MDI_VIEW_LOG,
	MDI_VIEW_REG,
	MDI_VIEW_BP,
	MDI_VIEW_MOD,
	MDI_VIEW_THREAD,
	MDI_VIEW_MEMORY,
	MDI_VIEW_STACK,

	MDI_DBG_STEP_INTO,
	MDI_DBG_STEP_OVER,
	MDI_DBG_RUN_TIL_RET,

	MDI_DBG_RUN,
	
	MDI_OPEN_MODULE,
	
	MDI_REFRESH,
	
	MDI_CHANGE_TITLE,
	MDI_CHANGE_POSITION,
	MDI_CHANGE_SIZE,
	MDI_CHILD_QUIT,
	MDI_ABOUT = wxID_ABOUT
};

#endif // __CHILDFRAME_H__
