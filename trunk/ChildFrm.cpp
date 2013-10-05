// ChildFrm.cpp : implementation of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "ChildFrm.h"
#include "DataView.h"

#include "MainFrm.h"

void CChildFrame::OnFinalMessage(HWND /*hWnd*/)
{
	delete this;
}

LRESULT CChildFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	m_pView = boost::shared_ptr<CDataView>(new CDataView);
	m_hWndClient = m_pView->Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	// Load the child window icon
	SetIcon(::LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MDICHILD)));
	
	bHandled = FALSE;
	return 1;
}

LRESULT CChildFrame::OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	LPMSG pMsg = (LPMSG)lParam;

	if (CMDIChildWindowImpl<CChildFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_pView->PreTranslateMessage(pMsg);
}

LRESULT CChildFrame::OnMDIActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Child to deactivate
	::SendMessage((HWND)wParam, WM_NCACTIVATE, FALSE, 0);

	// Child to activate
	::SendMessage((HWND)lParam, WM_NCACTIVATE, TRUE, 0);

	// Set focus to the MDI client
	::SetFocus(m_hWndMDIClient);
	
	// Switch to child window menu or back to default menu
	if((HWND)lParam == m_hWnd && m_hMenu != NULL)
	{
		HMENU hWindowMenu = GetStandardWindowMenu(m_hMenu);
		MDISetMenu(m_hMenu, hWindowMenu);
		MDIRefreshMenu();
		::DrawMenuBar(GetMainFrame());

		m_pMainFrame->m_pCurView = m_pView.get();
		m_pMainFrame->SetSettingView(m_pView->GetSourceSetting(), m_pView->GetDataSetting());
	}
	else if((HWND)lParam == NULL) { // last child has closed
		m_pMainFrame->m_pCurView = NULL;
		::SendMessage(GetMainFrame(), WM_MDISETMENU, 0, 0);
	}

	bHandled = FALSE;
	return 1;
}
