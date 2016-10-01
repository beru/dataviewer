// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CChildFrame : public CMDIChildWindowImpl<CChildFrame>
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MDICHILD)

	CChildFrame(class CMainFrame* pMainFrame)
		:
		m_pMainFrame(pMainFrame)
	{
	}

	class CMainFrame* m_pMainFrame;
	std::shared_ptr<class CDataView> m_pView;

	HWND GetMainFrame() { return ::GetParent(GetMDIFrame()); }

	LRESULT OnMDIActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Overridden system menu handler (upper left title bar icon)
	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
	{
		return ::SendMessage(GetMainFrame(), uMsg, wParam, lParam);
	}

	virtual void OnFinalMessage(HWND /*hWnd*/);

	BEGIN_MSG_MAP(CChildFrame)
		MESSAGE_HANDLER(WM_MDIACTIVATE, OnMDIActivate)
		MESSAGE_HANDLER(WM_MENUSELECT, OnMenuSelect)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
		CHAIN_MSG_MAP(CMDIChildWindowImpl<CChildFrame>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
};
