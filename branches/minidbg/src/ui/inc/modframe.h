#ifndef __MODFRAME_H__
#define __MODFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "childframe.h"
#include "listview.h"

class ModFrame : public ChildFrame
{
public:
	ModFrame(wxMDIParentFrame *parent, const wxString& title);
	~ModFrame();

public:
	void RefreshModList(ULONG sesId);

	void OnSize(wxSizeEvent& event);

private:
	virtual void SaveParams();
	virtual void LoadParams();

private:
	ListCtrl m_listCtrl;

private:
	DECLARE_EVENT_TABLE()
};

#endif // __MODFRAME_H__