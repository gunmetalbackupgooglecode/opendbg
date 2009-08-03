/*
    *    
    * Copyright (c) 2008 
    * d1mk4 <d1mk4@bk.ru> 
    *

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*! \file mainframe.cpp
* \brief This is implementation of main window
* \author d1mk4
*/

#include "precomp.h"

#include "mainframe.h"

IMPLEMENT_APP(App)

//! event-man of main window
BEGIN_EVENT_TABLE(MainFrame, wxMDIParentFrame)
	EVT_MENU(MDI_ABOUT, MainFrame::OnAbout)
	EVT_MENU(MDI_QUIT, MainFrame::OnQuit)
	EVT_MENU(MDI_OPEN_MODULE, MainFrame::OnOpenModule)

	EVT_MENU(MDI_VIEW_CPU, MainFrame::OnCPUFrame)
	EVT_MENU(MDI_VIEW_LOG, MainFrame::OnLogFrame)
	EVT_MENU(MDI_VIEW_REG, MainFrame::OnRegFrame)
	EVT_MENU(MDI_VIEW_BP, MainFrame::OnBPFrame)
	EVT_MENU(MDI_VIEW_MOD, MainFrame::OnModFrame)
	EVT_MENU(MDI_VIEW_THREAD, MainFrame::OnThreadFrame)
	EVT_MENU(MDI_VIEW_MEMORY, MainFrame::OnMemFrame)
	EVT_MENU(MDI_VIEW_STACK, MainFrame::OnStackFrame)

	EVT_MENU(MDI_DBG_RUN, MainFrame::OnRun)
	EVT_MENU(MDI_DBG_STEP_INTO, MainFrame::OnStepInto)
	EVT_MENU(MDI_DBG_STEP_OVER, MainFrame::OnStepOver)
	EVT_MENU(MDI_DBG_RUN_TIL_RET, MainFrame::OnTilRet)

	EVT_TRC_EXCEPT(wxID_ANY, MainFrame::OnExceptHandle)

	EVT_GUI_SNDTEXT(wxID_ANY, MainFrame::OnMsgBP)

	EVT_CLOSE(MainFrame::OnClose)
	EVT_SIZE(MainFrame::OnSize)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////////
MainFrame::MainFrame(wxWindow *parent,
                 const wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 const long style)
 : wxMDIParentFrame(parent, id, title, pos, size, style),
   m_trc_debug(wxEVT_TRC_DEBUG, wxID_ANY),
   m_trc_proc(wxEVT_TRC_PROCESS, wxID_ANY),
   m_trc_except(wxEVT_TRC_EXCEPT, wxID_ANY),
   m_userAnswer(0),
   m_debug_condition(m_debug_mutex),
   m_except_condition(m_except_mutex),
   m_proc_condition(m_proc_mutex),
   m_auimgr(this),
   m_cpuframe(new CPUFrame(this, wxT("CPU"))),
   m_logframe(new LogFrame(this, wxT("log"))),
   m_regframe(new RegFrame(this, wxT("registers"))),
   m_bpframe(new BPFrame(this, wxT("breakpoints"))),
   m_modframe(new ModFrame(this, wxT("executable modules"))),
   m_memframe(new MemFrame(this, wxT("memory"))),
   m_threadframe(new ThreadFrame(this, wxT("threads"))),
   m_stackframe(new StackFrame(this, wxT("stack")))
{
	CreateStatusBar();

	m_auimgr.AddPane(m_cpuframe.get(), wxAuiPaneInfo().Name(wxT("1")).Caption(wxT("CPU")).
		Left().PinButton());
	m_auimgr.AddPane(m_logframe.get(), wxAuiPaneInfo().Name(wxT("2")).Caption(wxT("Log")).
		Bottom().PinButton());
	m_auimgr.AddPane(m_regframe.get(), wxAuiPaneInfo().Name(wxT("3")).Caption(wxT("Registers")).
		Right().PinButton());
	m_auimgr.AddPane(m_bpframe.get(), wxAuiPaneInfo().Name(wxT("4")).Caption(wxT("Breakpoints")).
		Bottom().PinButton());
	m_auimgr.AddPane(m_modframe.get(), wxAuiPaneInfo().Name(wxT("5")).Caption(wxT("Modules")).
		Bottom().PinButton());
	m_auimgr.AddPane(m_memframe.get(), wxAuiPaneInfo().Name(wxT("6")).Caption(wxT("Memory")).
		Bottom().PinButton());
	m_auimgr.AddPane(m_threadframe.get(), wxAuiPaneInfo().Name(wxT("7")).Caption(wxT("Threads")).
		Right().PinButton());
	m_auimgr.AddPane(m_stackframe.get(), wxAuiPaneInfo().Name(wxT("8")).Caption(wxT("Stack")).
		Bottom().PinButton());

#if wxUSE_ACCEL
	// Accelerators
	wxAcceleratorEntry entries[3];
	entries[1].Set(wxACCEL_CTRL, (int) 'X', MDI_QUIT);
	entries[2].Set(wxACCEL_CTRL, (int) 'A', MDI_ABOUT);
	wxAcceleratorTable accel(3, entries);
	SetAcceleratorTable(accel);
#endif // wxUSE_ACCEL

	// Give it an icon
	SetIcon(wxICON(ngico));

	// Make a menubar
	wxMenu *file_menu = new wxMenu;
	wxMenu *view_menu = new wxMenu;
	wxMenu *help_menu = new wxMenu;
	wxMenu *dbg_menu = new wxMenu;

	wxMenuBar *menu_bar = new wxMenuBar;

#if wxUSE_TOOLBAR
	wxToolBar* dbgToolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition,
	                                   wxDefaultSize, wxTB_FLAT);
	wxToolBar* winToolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition,
	                                   wxDefaultSize, wxTB_FLAT);
	InitToolBar(dbgToolbar, winToolbar);
	m_auimgr.AddPane(dbgToolbar, wxAuiPaneInfo().Name(wxT("tb1")).Caption(wxT("Debug Toolbar")).ToolbarPane().Top().LeftDockable(false).RightDockable(false));
	m_auimgr.AddPane(winToolbar, wxAuiPaneInfo().Name(wxT("tb2")).Caption(wxT("Win Toolbar")).ToolbarPane().Top().LeftDockable(false).RightDockable(false));
#endif // wxUSE_TOOLBAR

	file_menu->Append(MDI_OPEN_MODULE, wxT("&Open...\tF3"), wxT("Open module for debug"));
	file_menu->Append(MDI_QUIT, wxT("&Exit\tAlt-X"), wxT("Quit the program"));

	view_menu->Append(MDI_VIEW_CPU, wxT("&CPU\tAlt-C"), wxT("Hide/Show CPU window"));
	view_menu->Append(MDI_VIEW_LOG, wxT("&Log\tAlt-L"), wxT("Hide/Show Log window"));
	view_menu->Append(MDI_VIEW_REG, wxT("&Registers\tAlt-R"), wxT("Hide/Show Registers window"));
	view_menu->Append(MDI_VIEW_BP, wxT("&Breakpoints\tAlt-B"), wxT("Hide/Show Breakpoints window"));
	view_menu->Append(MDI_VIEW_MOD, wxT("&Executable modules\tAlt-E"), wxT("Hide/Show Executable modules window"));
	view_menu->Append(MDI_VIEW_THREAD, wxT("&Threads\tAlt-T"), wxT("Hide/Show Threads window"));
	view_menu->Append(MDI_VIEW_MEMORY, wxT("&Memory\tAlt-M"), wxT("Hide/Show Memory window"));
	view_menu->Append(MDI_VIEW_STACK, wxT("&Stack\tAlt-S"), wxT("Hide/Show Stack window"));

	dbg_menu->Append(MDI_DBG_RUN, wxT("&Run\tF9"), wxT("Run program"));
	dbg_menu->AppendSeparator();
	dbg_menu->Append(MDI_DBG_STEP_INTO, wxT("Step &into\tF7"), wxT("Trace program step into"));
	dbg_menu->Append(MDI_DBG_STEP_OVER, wxT("Step &over\tF8"), wxT("Trace program step over"));
	dbg_menu->Append(MDI_DBG_RUN_TIL_RET, wxT("Execute till return\tCtrl-F9"), wxT("Execute program till return"));

	help_menu->Append(MDI_ABOUT, _T("&About\tF1"));

	menu_bar->Append(file_menu, wxT("&File"));
	menu_bar->Append(view_menu, wxT("&View"));
	menu_bar->Append(dbg_menu, wxT("&Debug"));
	menu_bar->Append(help_menu, wxT("&Help"));

	// Associate the menu bar with the frame
	SetMenuBar(menu_bar);

	// load layout all frames in window and position of main window
	LoadLayout();

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
MainFrame::~MainFrame()
{
	SaveLayout();

	m_auimgr.UnInit();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::SaveLayout()
{
	wxConfigBase *pConfig = wxConfigBase::Get();

	if ( pConfig == NULL )
		return;

	{
		wxSize size = GetClientSize();
		wxPoint pos = GetPosition();
		wxString path = wxT("CommonSettings/MainCoords");

		// если пытаемся сохранить координаты свернутого окна
		// сохраняем предыдущие нормальные координаты
		if(this->IsIconized())
		{
			pos.x = pConfig->Read(path + wxT("/x"), 50);
			pos.y = pConfig->Read(path + wxT("/y"), 50);
			size.SetWidth(pConfig->Read(path + wxT("/w"), 350));
			size.SetHeight(pConfig->Read(path + wxT("/h"), 200));
		}

		pConfig->Write(path + wxT("/x"), (long) pos.x);
		pConfig->Write(path + wxT("/y"), (long) pos.y);
		pConfig->Write(path + wxT("/w"), (long) size.GetX());
		pConfig->Write(path + wxT("/h"), (long) size.GetY());
	}

	{
		wxString childPath = wxT("CommonSettings/ChildCoords");
		wxString windowsSettings = m_auimgr.SavePerspective();

		if ( !pConfig->Write(childPath, windowsSettings) )
			wxLogError(wxT("Save ") + childPath + wxT(" is failed"));
	}
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::LoadLayout()
{
	wxConfigBase *pConfig = wxConfigBase::Get();

	if ( pConfig == NULL )
		return;

	{
		wxPoint pos;
		wxSize size;
		wxString path = wxT("CommonSettings/MainCoords");

		pos.x = pConfig->Read(path + wxT("/x"), 50);
		pos.y = pConfig->Read(path + wxT("/y"), 50);
		size.SetWidth(pConfig->Read(path + wxT("/w"), 350));
		size.SetHeight(pConfig->Read(path + wxT("/h"), 200));

		Move(pos.x, pos.y);
		SetClientSize(size.GetWidth(), size.GetHeight());
	}

	{
		wxString childPath = wxT("CommonSettings/ChildCoords");
		wxString windowsSettings;

		if ( !pConfig->Read(childPath, &windowsSettings) )
		{
			wxLogError(wxT("Load ") + childPath + wxT(" is failed"));
			ShowUsableWindows();
		}

		if ( !m_auimgr.LoadPerspective(windowsSettings) )
			wxLogError(childPath + wxT("not loaded to window manager"));
	}

}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnClose(wxCloseEvent& event)
{
	event.Skip();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnSize(wxSizeEvent&
                                  #ifdef __WXUNIVERSAL__
                                  event
                                  #else
                                  WXUNUSED(event)
                                  #endif
                                  )
{
    int w, h;
    GetClientSize(&w, &h);

    GetClientWindow()->SetSize(0, 0, w , h);

    // FIXME: On wxX11, we need the MDI frame to process this
    // event, but on other platforms this should not
    // be done.
#ifdef __WXUNIVERSAL__
    event.Skip();
#endif
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnCPUFrame( wxCommandEvent& event )
{
	if (m_auimgr.GetPane(m_cpuframe.get()).IsShown())
		m_auimgr.GetPane(m_cpuframe.get()).Show(false);
	else
		m_auimgr.GetPane(m_cpuframe.get()).Show(true);

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnLogFrame( wxCommandEvent& event )
{
	if (m_auimgr.GetPane(m_logframe.get()).IsShown())
		m_auimgr.GetPane(m_logframe.get()).Show(false);
	else
		m_auimgr.GetPane(m_logframe.get()).Show(true);

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnRegFrame( wxCommandEvent& event )
{
	if (m_auimgr.GetPane(m_regframe.get()).IsShown())
		m_auimgr.GetPane(m_regframe.get()).Show(false);
	else
		m_auimgr.GetPane(m_regframe.get()).Show(true);

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnBPFrame( wxCommandEvent& event )
{
	if (m_auimgr.GetPane(m_bpframe.get()).IsShown())
		m_auimgr.GetPane(m_bpframe.get()).Show(false);
	else
		m_auimgr.GetPane(m_bpframe.get()).Show(true);

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnModFrame( wxCommandEvent& event )
{
	if (m_auimgr.GetPane(m_modframe.get()).IsShown())
		m_auimgr.GetPane(m_modframe.get()).Show(false);
	else
		m_auimgr.GetPane(m_modframe.get()).Show(true);

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnThreadFrame( wxCommandEvent& event )
{
	if (m_auimgr.GetPane(m_threadframe.get()).IsShown())
		m_auimgr.GetPane(m_threadframe.get()).Show(false);
	else
		m_auimgr.GetPane(m_threadframe.get()).Show(true);

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnMemFrame( wxCommandEvent& event )
{
	if (m_auimgr.GetPane(m_memframe.get()).IsShown())
		m_auimgr.GetPane(m_memframe.get()).Show(false);
	else
		m_auimgr.GetPane(m_memframe.get()).Show(true);

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnStackFrame( wxCommandEvent& event )
{
	if (m_auimgr.GetPane(m_stackframe.get()).IsShown())
		m_auimgr.GetPane(m_stackframe.get()).Show(false);
	else
		m_auimgr.GetPane(m_stackframe.get()).Show(true);

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnOpenModule( wxCommandEvent& event )
{
	wxFileDialog dialog(this, wxT("Open module for debug"),
	wxGetCwd(),
	wxEmptyString,
	wxT("EXE files (*.exe)|*.exe|DLL files (*.dll)|*.dll"));

	if(dialog.ShowModal() == wxID_OK)
	{
		// set window title with current module name
		this->SetTitle(this->GetTitle() + " - " + dialog.GetFilename());

		if (m_debugger.m_sesId > 0)
			m_debugger.close_session();

		m_debugger.open_session(TRC_SESSION_LOCAL, NULL);

		// set callback dispatchers for cooperation with tracer
		m_debugger.set_callback(TRC_DBG_EVENTS_CALLBACK, thunk_dbg_event, this);
		m_debugger.set_callback(TRC_PROCESS_EVENTS_CALLBACK, thunk_proc_event, this);
		m_debugger.set_callback(TRC_EXCEPTIONS_CALLBACK, thunk_except_event, this);

		// TODO: make settings dialog like ollydbg
		m_debugger.load(dialog.GetPath(), TRC_OPT_BREAK_ON_EP | TRC_OPT_BREAK_ON_SEH_HANDLER);

		//CONTEXT ctx;
		//PTHREAD_LIST threadList = trc_get_thread_list(m_debugger.m_sesId);
		//trc_get_thread_ctx(m_debugger.m_sesId, threadList->thread[0].TID, &ctx);
		//m_memframe->OutputMemory(m_debugger.m_sesId, ctx.Eip);
		//m_stackframe->OutputMemory(m_debugger.m_sesId, ctx.Esp);
	}
}

#if wxUSE_TOOLBAR
//////////////////////////////////////////////////////////////////////////
void MainFrame::InitToolBar( wxToolBar* dbgToolbar, wxToolBar* winToolBar )
{
	wxBitmap bitmaps[12];

	bitmaps[0] = wxBitmap( wxBITMAP(OPEN) );
	bitmaps[1] = wxBitmap( wxBITMAP(RUN) );
	bitmaps[2] = wxBitmap( wxBITMAP(STEPIN) );
	bitmaps[3] = wxBitmap( wxBITMAP(STEPOVER) );
	bitmaps[4] = wxBitmap( wxBITMAP(TILLRET) );
	bitmaps[5] = wxBitmap( wxBITMAP(LOG) );
	bitmaps[6] = wxBitmap( wxBITMAP(MODULES) );
	bitmaps[7] = wxBitmap( wxBITMAP(MEMORY) );
	bitmaps[8] = wxBitmap( wxBITMAP(THREADS) );
	bitmaps[9] = wxBitmap( wxBITMAP(CPU) );
	bitmaps[10] = wxBitmap( wxBITMAP(STACK) );
	bitmaps[11] = wxBitmap( wxBITMAP(REFERS) );

	dbgToolbar->AddTool(MDI_OPEN_MODULE, wxT("Open"), bitmaps[0], wxT("Open module for debuging"));
	dbgToolbar->AddTool(MDI_DBG_RUN, wxT("Run"), bitmaps[1], wxT("Run module"));
	dbgToolbar->AddTool(MDI_DBG_STEP_INTO, wxT("Step into"), bitmaps[2], wxT("Step into"));
	dbgToolbar->AddTool(MDI_DBG_STEP_OVER, wxT("Step over"), bitmaps[3], wxT("Step over"));
	dbgToolbar->AddTool(MDI_DBG_RUN_TIL_RET, wxT("Execute till return"), bitmaps[4], wxT("Execute till return"));
	dbgToolbar->Realize();

	winToolBar->AddTool(MDI_VIEW_LOG, wxT("Show Log Window"), bitmaps[5], wxT("Show Log Window (Alt-L)"));
	winToolBar->AddTool(MDI_VIEW_MOD, wxT("Show Modules Window"), bitmaps[6], wxT("Show Modules Window (Alt-E)"));
	winToolBar->AddTool(MDI_VIEW_MEMORY, wxT("Show Memory Window"), bitmaps[7], wxT("Show Memory Window (Alt-M)"));
	winToolBar->AddTool(MDI_VIEW_THREAD, wxT("Show Thread Window"), bitmaps[8], wxT("Show Thread Window (Alt-T)"));
	winToolBar->AddTool(MDI_VIEW_CPU, wxT("Show CPU Window"), bitmaps[9], wxT("Show CPU Window (Alt-C)"));
	winToolBar->AddTool(MDI_VIEW_STACK, wxT("Show Stack Window"), bitmaps[10], wxT("Show Stack Window (Alt-S)"));
	winToolBar->AddTool(MDI_VIEW_REG, wxT("Show Registers Window"), bitmaps[11], wxT("Show Registers Window (Alt-R)"));
	winToolBar->Realize();
}
#endif // wxUSE_TOOLBAR

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnStepInto( wxCommandEvent& event )
{
	m_debugger.m_trace_mode = TRC_STEP_INTO;
	m_debug_condition.Signal();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnStepOver( wxCommandEvent& event )
{
	m_debugger.m_trace_mode = TRC_STEP_OVER;
	m_debug_condition.Signal();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnTilRet( wxCommandEvent& event )
{
	m_debugger.m_trace_mode = TRC_RUN_TIL_RET;
	m_debug_condition.Signal();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnRun( wxCommandEvent& event )
{
	m_debugger.m_trace_mode = TRC_RUN;
	m_debug_condition.Signal();
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::OnExceptHandle(TrcExceptEvent &event)
{
	if( wxMessageBox(wxT("Handle this exception?"), wxT("Exception"), wxYES | wxNO) == wxID_YES )
		m_userAnswer = 1;
	else
		m_userAnswer = 0;

	m_except_condition.Signal();
}

//////////////////////////////////////////////////////////////////////////
ULONG __stdcall MainFrame::thunk_dbg_event( PTRC_EXCEPTION_EVENT evt, void *arg )
{
	MainFrame* pthis = static_cast<MainFrame*>(arg);
	pthis->m_trc_debug.setDebugEvent(*evt);
	pthis->m_trc_debug.setSesId(pthis->m_debugger.m_sesId);

	pthis->m_modframe->RefreshModList(pthis->m_debugger.m_sesId);
	pthis->m_threadframe->RefreshThreadList(pthis->m_debugger.m_sesId);

	wxPostEvent(pthis->m_cpuframe->GetEventHandler(), pthis->m_trc_debug);
	wxPostEvent(pthis->m_logframe->GetEventHandler(), pthis->m_trc_debug);
	wxPostEvent(pthis->m_regframe->GetEventHandler(), pthis->m_trc_debug);
	wxPostEvent(pthis->m_memframe->GetEventHandler(), pthis->m_trc_debug);
	wxPostEvent(pthis->m_stackframe->GetEventHandler(), pthis->m_trc_debug);

	pthis->m_debug_condition.Wait();

	return static_cast<MainFrame*>(arg)->m_debugger.get_tracemode();
}

/////////////////////////////////////////////////////////////////////////
ULONG __stdcall MainFrame::thunk_proc_event( PTRC_PROCESS_EVENT evt, void *arg )
{
	MainFrame* pthis = static_cast<MainFrame*>(arg);
	pthis->m_trc_proc.setProcEvent(*evt);
	pthis->m_trc_proc.setSesId(pthis->m_debugger.m_sesId);

	wxPostEvent(pthis->m_logframe->GetEventHandler(), pthis->m_trc_proc);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
ULONG __stdcall MainFrame::thunk_except_event( PTRC_EXCEPTION_EVENT evt, void *arg )
{
	MainFrame* pthis = static_cast<MainFrame*>(arg);
	pthis->m_trc_except.setExceptEvent(*evt);
	pthis->m_trc_except.setSesId(pthis->m_debugger.m_sesId);

	wxPostEvent(pthis->GetEventHandler(), pthis->m_trc_except);
	wxPostEvent(pthis->m_logframe->GetEventHandler(), pthis->m_trc_except);

	pthis->m_except_condition.Wait();

	if(pthis->m_userAnswer)
		return TRC_EXC_HANDLE;
	else
		return TRC_EXC_NOT_HANDLE;
}

//////////////////////////////////////////////////////////////////////////
int MainFrame::InitDebugger()
{
	return m_debugger.init();
}

//////////////////////////////////////////////////////////////////////////
//! controller-function receives break-point events and sends it to BPFrame
void MainFrame::OnMsgBP( GuiProcEvent& event )
{
	GuiProcEvent evt(wxEVT_GUI_TEXT2WINDOW, wxID_ANY);
	
	switch( event.GetId() )
	{
	case GUI_MSG_ADD_BP:
		{
			UCHAR addrbp = htodw((char*)event.getAddrBP().c_str());
			trc_set_bp(m_trc_debug.getSesId(), TRC_BP_SOFTWARE,
				(UCHAR)m_trc_debug.getDebugEvent().CurrentThread->TID, (PVOID)addrbp, NULL);

			evt.setDebugEvent(m_trc_debug.getDebugEvent());
			evt.setSesId(m_trc_debug.getSesId());
			evt.setAddrBP(event.getAddrBP());
			evt.setDisasmBP(event.getDisasmBP());
			evt.SetId(GUI_MSG_ADD_BP);
			break;
		}

	case GUI_MSG_DEL_BP:
		{
			UCHAR addrbp = htodw((char*)event.getAddrBP().c_str());
			trc_delete_bp(m_trc_debug.getSesId(),
				(UCHAR)m_trc_debug.getDebugEvent().CurrentThread->TID, (PVOID)addrbp, TRC_BP_SOFTWARE);

			evt.setDebugEvent(m_trc_debug.getDebugEvent());
			evt.setSesId(m_trc_debug.getSesId());
			evt.setAddrBP(event.getAddrBP());
			evt.setDisasmBP(event.getDisasmBP());
			evt.SetId(GUI_MSG_DEL_BP);
			break;
		}

	default:
		evt.SetId(wxID_ANY);
		break;
	}
	
	wxPostEvent(this->m_bpframe->GetEventHandler(), evt);
}

//////////////////////////////////////////////////////////////////////////
void MainFrame::ShowUsableWindows()
{
	m_auimgr.GetPane(m_cpuframe.get()).Show(true);
	m_auimgr.GetPane(m_regframe.get()).Show(true);
	m_auimgr.GetPane(m_memframe.get()).Show(true);
	m_auimgr.GetPane(m_stackframe.get()).Show(true);

	m_auimgr.GetPane(m_logframe.get()).Show(false);
	m_auimgr.GetPane(m_bpframe.get()).Show(false);
	m_auimgr.GetPane(m_modframe.get()).Show(false);
	m_auimgr.GetPane(m_threadframe.get()).Show(false);

	m_auimgr.Update();
}

//////////////////////////////////////////////////////////////////////////
//
//! Application's entry point
//
//! This functions defines important startup settings of the main window
// ---------------------------------------------------------------------------
bool App::OnInit()
{
	wxConfigBase::Set( new wxFileConfig(wxT("NGDebugger"), wxEmptyString,
		wxGetCwd() + wxT("\\ngdbg.ini")) );

	wxFileName filename;
	filename.MakeAbsolute();

	MainFrame *frame = new MainFrame((wxFrame *)NULL, wxID_ANY, wxT("NGDebugger v0.002 alpha"),
	                    wxDefaultPosition, wxDefaultSize,
	                    wxDEFAULT_FRAME_STYLE | wxCLIP_SIBLINGS | wxHSCROLL | wxVSCROLL);

	// startup debugger engine
	if( frame->InitDebugger() > 0 )
	{
		frame->Show(true);
		SetTopWindow(frame);
	}
	// else output error message and close the window
	else
	{
		wxMessageBox(wxT("dbgapi.sys not found!"), wxT("Error"), wxOK | wxICON_ERROR);
		frame->Close();
	}

	return true;
}
