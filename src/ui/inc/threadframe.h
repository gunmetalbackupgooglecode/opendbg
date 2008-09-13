#ifndef __THREADFRAME_H__
#define __THREADFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "childframe.h"
#include "listview.h"


class ThreadFrame : public ChildFrame
{
public:
	ThreadFrame(wxMDIParentFrame *parent, const wxString& title);
	~ThreadFrame();

	void RefreshThreadList(ULONG sesId);

	void OnSize(wxSizeEvent& event);

private:
	virtual void SaveParams();
	virtual void LoadParams();

private:
	ListCtrl m_listCtrl;

private:
	DECLARE_EVENT_TABLE()
};

#endif // __THREADFRAME_H__