#pragma once

struct SourceSetting;
struct IDataSetting;

class CMainFrame
	:
	public CMDIFrameWindowImpl<CMainFrame>,
	public CUpdateUI<CMainFrame>,
	public CMessageFilter,
	public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
	
	CMainFrame();

	CMDICommandBarCtrl m_CmdBar;
	CSplitterWindow m_splitter;
	CPaneContainer m_painContainer;
	std::shared_ptr<class CSettingDialog> m_pSettingDlg;
	class CDataView* m_pCurView;
	
	HWND CreateClient();

	void ReadData(const std::shared_ptr<SourceSetting>& pSrcSetting, std::shared_ptr<IDataSetting>& pDataSetting);
	void ProcessData(const std::shared_ptr<SourceSetting>& pSrcSetting, std::shared_ptr<IDataSetting>& pDataSetting);
	void SetSettingView(const SourceSetting* pSrcSetting, const IDataSetting* pDataSetting);
	void updateUI();
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();
	
	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_EDIT_COPY, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_EDIT_PASTE, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_HAND, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
		UPDATE_ELEMENT(ID_ZOOM, UPDUI_MENUPOPUP|UPDUI_TOOLBAR)
	END_UPDATE_UI_MAP()
	
	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER_EX(ID_VIEW_ZOOMIN, OnViewZoomin)
		COMMAND_ID_HANDLER_EX(ID_VIEW_ZOOMOUT, OnViewZoomout)
		COMMAND_ID_HANDLER_EX(ID_VIEW_ZOOMRESET, OnViewZoomreset)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_WINDOW_CASCADE, OnWindowCascade)
		COMMAND_ID_HANDLER(ID_WINDOW_TILE_HORZ, OnWindowTile)
		COMMAND_ID_HANDLER_EX(ID_HAND, OnHand)
		COMMAND_ID_HANDLER_EX(ID_ZOOM, OnZoom)
		COMMAND_ID_HANDLER_EX(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER_EX(ID_EDIT_PASTE, OnEditPaste)
		COMMAND_ID_HANDLER_EX(ID_EDIT_COPYIMAGE, OnEditCopyImage)

//		COMMAND_ID_HANDLER(ID_WINDOW_ARRANGE, OnWindowArrangeIcons)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CMDIFrameWindowImpl<CMainFrame>)
		CHAIN_MDI_CHILD_COMMANDS()
	END_MSG_MAP()
	
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewZoomin(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnViewZoomout(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnViewZoomreset(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWindowArrangeIcons(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnHand(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnZoom(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnEditCopy(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnEditPaste(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnEditCopyImage(WORD wNotifyCode, WORD wID, HWND hWndCtl);
};
