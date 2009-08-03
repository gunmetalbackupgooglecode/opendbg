#ifndef __STACKFRAME_H__
#define __STACKFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "events.h"
#include "debugger.h"
#include "childframe.h"
#include "MemCtrl/MemCtrl.h"

class StackFrame : public ChildFrame
{
public:
	StackFrame(wxMDIParentFrame *parent, const wxString& title);
	~StackFrame();

	void OutputMemory(ULONG sesId, ULONG mem_addr);
	
	void RefreshMemory(TrcDebugEvent& event);
	void OnSize(wxSizeEvent& event);

private:
	virtual void SaveParams();
	virtual void LoadParams();

private:
	ListTextCtrl m_stackctrl;

private:
	DECLARE_EVENT_TABLE()
};

#endif // __STACKFRAME_H__