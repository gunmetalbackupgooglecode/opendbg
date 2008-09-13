#ifndef __CPUFRAME_H__
#define __CPUFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "cpulistview.h"
#include "childframe.h"
#include "events.h"

class CPUFrame : public ChildFrame
{
public:
	CPUFrame();
	CPUFrame(wxMDIParentFrame *parent, const wxString& title);
	~CPUFrame();

	void OnItemRClick(wxListEvent& event);

	void OnTest(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);

	void AddItem(TrcDebugEvent& event);
	void ShowContextMenu(const wxPoint& pos);

private:
	virtual void SaveParams();
	virtual void LoadParams();

private:
	CPUListCtrl m_listCtrl;

private:
	DECLARE_EVENT_TABLE()
};

#endif // __CPUFRAME_H__