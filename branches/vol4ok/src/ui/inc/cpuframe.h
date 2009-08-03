/*! \file bpframe.h
* \brief Header of CPU frame

This frame provides all functionality for work with code's dump,
break points and some other
* \author d1mk4
*/

#ifndef __CPUFRAME_H__
#define __CPUFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "cpulistview.h"
#include "childframe.h"
#include "events.h"

/*! \class CPUFrame cpuframe.h "src/ui/inc/cpuframe.h"
*   \brief This frame is main window for work with debugger
*
* This frame displays all important information about debugging process
*/
class CPUFrame : public ChildFrame
{
public:
	CPUFrame(); //< Default constructor
	CPUFrame(wxMDIParentFrame *parent, const wxString& title); //< Window constructor
	~CPUFrame(); //< destructor

	/*!
	This handler processes all right button clicks on the listview items
	@param[in] event listview event
	*/
	void OnItemRClick(wxListEvent& event);

	/*!
	I don't remember what is it =(
	*/
	void OnTest(wxCommandEvent& event);

	/*!
	This handler processes window resizing events
	@param[in] event size event
	*/
	void OnSize(wxSizeEvent& event);

	/*!
	This handler processes all item adding events
	*/
	void AddItem(TrcDebugEvent& event);

	/*!
	This function pop-ups context menu
	*/
	void ShowContextMenu(const wxPoint& pos);

private:
	virtual void SaveParams();
	virtual void LoadParams();

private:
	CPUListCtrl m_listCtrl; //< listview thunk

private:
	DECLARE_EVENT_TABLE()
};

#endif // __CPUFRAME_H__