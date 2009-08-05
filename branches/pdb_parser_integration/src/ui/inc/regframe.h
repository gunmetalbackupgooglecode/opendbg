#ifndef __REGFRAME_H__
#define __REGFRAME_H__

#if _MSC_VER > 1000
	#pragma once
#endif // _MSC_VER > 1000

#include "precomp.h"

#include "events.h"
#include "childframe.h"

// subclassing for catch richtextctrl events
class RegRichTextCtrl : public wxRichTextCtrl
{
	typedef CONTEXT Context;

	DECLARE_EVENT_TABLE();

public:
	RegRichTextCtrl(wxWindow* parent);

public:
	void OnLeftClick(wxMouseEvent& event);
	void OnLeftDClick(wxMouseEvent& event);
	void OnMoveMouse(wxMouseEvent& event);
	void OnChar(wxKeyEvent& event);

public:
	void SetContextId(int sesId);
	int GetContextId();

	void SetThreadId(int tid);
	int GetThreadId();

public:
	void OutputBasic( const Context &ctx );
	void OutputDR( const Context &ctx );
	void EditRegister(const char *RegNames, const wxString &value);
	void WhatBasicRegister(const char *RegNames, int val, Context &ctx);

private:
	int m_sesId;
	int m_tid;
};

class RegFrame : public ChildFrame
{
	typedef CONTEXT Context;

	DECLARE_EVENT_TABLE()

public:
	RegFrame(wxMDIParentFrame* parent, const wxString& title);

	// деструктор
	~RegFrame();

public:
	// event handlers
	void OnText(wxRichTextEvent& event);

public:
	// output all register values to RegFrame
	void Output(TrcDebugEvent& event);
	void OnSize(wxSizeEvent& event);

	int GetContextId();

private:
	void OutputBasic(const Context &ctx);
	void OutputDR(const Context &ctx);
	void OutputMMX(const Context &ctx);
	void Output3dnow(const Context &ctx);
	void OutputFPU(const Context &ctx);

private:
	virtual void SaveParams();
	virtual void LoadParams();

public:
	wxPanel* MakePageDR();
	wxPanel* MakePageMMX();
	wxPanel* MakePageFPU();
	wxPanel* MakePage3dnow();

private:
	
	RegRichTextCtrl *m_textCtrlBasic;
	RegRichTextCtrl *m_textCtrlDR;
	RegRichTextCtrl *m_textCtrlMMX;
	RegRichTextCtrl *m_textCtrlFPU;
	RegRichTextCtrl *m_textCtrl3dnow;


	//wxBookCtrlBase *m_bookCtrl;
	wxAuiNotebook *m_bookCtrl;
	wxBoxSizer *m_sizerFrame;
	wxPanel *m_panel;
	int m_textFlags;

	int m_sesId;
	int m_tid;
};

#endif // __REGFRAME_H__