#ifndef __BPFRAME_H__
#define __BPFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "childframe.h"

class GuiProcEvent;
class ListCtrl;

class BPFrame : public ChildFrame
{
public:
	// конструктор окна
	BPFrame(wxMDIParentFrame *parent, const wxString& title);

	// деструктор
	~BPFrame();

	void OnAddBP(GuiProcEvent& event);
	void OnDelBP(GuiProcEvent& event);
	void OnSize(wxSizeEvent& event);

private:
	virtual void SaveParams();
	virtual void LoadParams();

private:
	ListCtrl m_listCtrl;

private:
	DECLARE_EVENT_TABLE()
};

#endif // __BPFRAME_H__