/*! \file childframe.h 
* \brief Header of child frame for inheritance
* \author d1mk4
*/

#ifndef __CHILDFRAME_H__
#define __CHILDFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "listview.h"

/*! \class ChildFrame childframe.h "src/ui/inc/childframe.h"
*   \brief This frame created for inheritence
*
* This frame provides basic modified handlers
*/
class ChildFrame : public wxControl
{
public:
	/*!
	ChildFrame constructor
	@param[in] parent MDI parent window
	@param[in] title  title name for new window
	*/
	ChildFrame(wxMDIParentFrame *parent, const wxString& title);

	/*!
	ChildFrame virtual destructor
	*/
	virtual ~ChildFrame();

	/*!
	Handler for activating window when user click on it
	@param[in] event event from window subsystem
	*/
	void OnActivate(wxActivateEvent& event);

	/*!
	Handler for refresh window
	@param[in] event event from window subsystem
	*/
	void OnRefresh(wxCommandEvent& event);

	/*!
	Handler for refresh window
	@param[in] event event from window subsystem
	*/
	void OnUpdateRefresh(wxUpdateUIEvent& event);

	/*!
	Handler for change window title
	@param[in] event event from window subsystem
	*/
	void OnChangeTitle(wxCommandEvent& event);

	/*!
	Handler for chaning window position
	@param[in] event event from window subsystem
	*/
	void OnChangePosition(wxCommandEvent& event);

	/*!
	Handler for changing window size
	@param[in] event event from window subsystem
	*/
	void OnChangeSize(wxCommandEvent& event);

	/*!
	Handler for killing window
	@param[in] event event from window subsystem
	*/
	void OnQuit(wxCommandEvent& event);

	/*!
	Handler for resizing window size
	@param[in] event event from window subsystem
	*/
	void OnSize(wxSizeEvent& event);

	/*!
	Handler for moving window
	@param[in] event event from window subsystem
	*/
	void OnMove(wxMoveEvent& event);

	/*!
	Handler for closing window
	@param[in] event event from window subsystem
	*/
	void OnClose(wxCloseEvent& event);

	/*!
	Save window coordinates and maybe some other window parameters
	*/
	virtual void SaveParams() = 0;

	/*!
	Load window coordinates and maybe some other window parameters
	*/
	virtual void LoadParams() = 0;

	/*!
	Save window coordinates
	*/
	virtual void SaveWinCoord(const wxString &path);

	/*!
	Load window coordinates
	*/
	virtual void LoadWinCoord(const wxString &path);

	/*!
	Handler for process pasting from buffer
	*/
	void OnPaste(wxCommandEvent& event);

	/*!
	Handler for process pasting from buffer when GUI is refreshed
	*/
	void OnUpdatePaste(wxUpdateUIEvent& event);

	DECLARE_EVENT_TABLE()
};

/*!
Enum contains menu item ids
*/
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
