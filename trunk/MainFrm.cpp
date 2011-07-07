// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "ChildFrm.h"
#include "MainFrm.h"

#include "SettingDialog.h"
#include "DataView.h"

CMainFrame::CMainFrame()
	:
	m_pCurView(NULL)
{
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CMDIFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	HWND hWnd = MDIGetActive();
	if(hWnd != NULL)
		return (BOOL)::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM)pMsg);

	return FALSE;
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

HWND CMainFrame::CreateClient()
{
	// Get the Client RECT for the entire window as a starting size
	RECT rcClient;
	GetClientRect(&rcClient);
	
	// Create the vertical splitter. This is the main window
	m_splitter.Create(m_hWnd, rcClient, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_splitter.SetSplitterExtendedStyle(0);
	m_painContainer.Create(m_splitter, _T("setting"));
	m_painContainer.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);
	
	// Create the Setting dialog in the left pane
	m_pSettingDlg = boost::shared_ptr<CSettingDialog>(new CSettingDialog);
	m_pSettingDlg->Create(m_painContainer);
	m_pSettingDlg->m_readDelegate.bind(this, &CMainFrame::ReadData);
	m_pSettingDlg->m_processDelegate.bind(this, &CMainFrame::ProcessData);
	
	m_painContainer.SetClient(*m_pSettingDlg);
	m_splitter.SetSplitterPane(SPLIT_PANE_LEFT, m_painContainer);
	
	// Create the MDI client in the right pane
	m_hWndMDIClient = CreateMDIClient();
	m_splitter.SetSplitterPane(SPLIT_PANE_RIGHT, m_hWndMDIClient);
	
	// IMPORTANT! Make the splitter the parent of the MDI client
	::SetParent(m_hWndMDIClient, m_splitter.m_hWnd);
	
	m_splitter.SetSplitterPos(400); // from left
	
	// Splitter is ultimately the client of Main Frame (m_hWndClient)
	return m_splitter.m_hWnd;
	
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	CreateSimpleStatusBar();

	m_hWndClient = CreateClient();

	m_CmdBar.SetMDIClient(m_hWndMDIClient);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CChildFrame* pChild = new CChildFrame(this);
	pChild->CreateEx(m_hWndMDIClient);

	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MDICascade();
	return 0;
}

LRESULT CMainFrame::OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MDITile();
	return 0;
}

LRESULT CMainFrame::OnWindowArrangeIcons(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MDIIconArrange();
	return 0;
}

void CMainFrame::SetSettingView(const ProcessSetting& setting)
{
	m_pSettingDlg->SetSetting(setting);
	
}

void CMainFrame::ReadData(const ProcessSetting& setting)
{
	if (!m_pCurView)
		return;
	
	m_pCurView->ReadData(setting);
	
}

void CMainFrame::ProcessData(const ProcessSetting& setting)
{
	if (!m_pCurView)
		return;
	
	m_pCurView->ProcessData(setting);
	
}

