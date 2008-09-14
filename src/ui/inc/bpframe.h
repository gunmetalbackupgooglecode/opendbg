/*! \file bpframe.h
* \brief Header of break-point frame
* \author d1mk4
*/

#ifndef __BPFRAME_H__
#define __BPFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "childframe.h"

// forward declarations
class GuiProcEvent;
class ListCtrl;

/*! \class BPFrame bpframe.h "src/ui/inc/bpframe.h"
*   \brief This frame shows break-point list
*
* This frame provides UI functions for work with break-points.
*/
class BPFrame : public ChildFrame
{
public:
	/*!
	BPFrame constructor
	@param[in] parent MDI parent window
	@param[in] title  title name for new window
	*/
	BPFrame(wxMDIParentFrame *parent, const wxString& title);

	/*!
	BPFrame destructor
	*/
	~BPFrame();

	/*!
	Handler for adding new break-points to the window
	@param[in] event event from user
	*/
	void OnAddBP(GuiProcEvent& event);

	/*!
	Handler for deleting break-points from the window
	@param[in] event event from user
	*/
	void OnDelBP(GuiProcEvent& event);

	/*!
	Hander for resizing bpframe window
	@param[in] event size event from window subsystem
	*/
	void OnSize(wxSizeEvent& event);

private:

	/*!
	Save window coordinates and maybe some other window parameters
	*/
	virtual void SaveParams();

	/*!
	Load window coordinates and maybe some other window parameters
	*/
	virtual void LoadParams();

private:
	ListCtrl m_listCtrl; ///< @brief ListView object

private:
	DECLARE_EVENT_TABLE()
};

#endif // __BPFRAME_H__