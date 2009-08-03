#ifndef __MEMFRAME_H__
#define __MEMFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "events.h"
#include "debugger.h"
#include "childframe.h"
#include "MemCtrl/MemCtrl.h"

class MemFrame : public ChildFrame
{
	DECLARE_EVENT_TABLE()

public:
	MemFrame(wxMDIParentFrame *parent, const wxString& title);
	~MemFrame();

	void OutputMemory(uint32_t sesId, uint32_t mem_addr);

	void RefreshMemory(TrcDebugEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnScroll(wxScrollWinEvent& event);
	void OnPaint(wxPaintEvent& event);
	void OnChar(wxKeyEvent& event);

private:
	virtual void SaveParams();
	virtual void LoadParams();

private:
	ListTextCtrl *m_memctrl;
	wxBoxSizer *m_sizerFrame;
	wxPanel *m_panel;
};

#endif // __MEMFRAME_H__