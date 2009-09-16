#pragma once

#include "resource.h"       // main symbols
#include "AutoCombo.h"

struct ProcessSetting;

class CSettingDialog : 
	public CDialogImpl<CSettingDialog>,
	public CWinDataExchange<CSettingDialog>,
	public CMessageFilter,
	public CIdleHandler
{
public:
	CSettingDialog();
	~CSettingDialog();
	enum { IDD = IDD_DLG_SETTING };
	
	BEGIN_DDX_MAP(CSettingDialog)
		DDX_CONTROL(IDC_CMB_IMAGENAME, m_wndCmbImageName)
		DDX_CONTROL_HANDLE(IDC_TAB, m_wndTab)
	END_DDX_MAP()
	
	BEGIN_MSG_MAP(CSettingDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MSG_WM_TIMER(OnTimer)
		COMMAND_HANDLER(IDC_BTN_ENUMERATE, BN_CLICKED, OnBnClickedBtnEnumerate)
		COMMAND_HANDLER(IDC_BTN_READ, BN_CLICKED, OnBnClickedBtnRead)
		COMMAND_HANDLER(IDC_CHK_READ_AUTO, BN_CLICKED, OnBnClickedChkReadAuto)
		COMMAND_HANDLER(IDC_BTN_PROCESS, BN_CLICKED, OnBnClickedBtnProcess)
		COMMAND_HANDLER_EX(IDC_BTN_COPY, BN_CLICKED, OnBtnCopyBnClicked)
		COMMAND_HANDLER_EX(IDC_BTN_PASTE, BN_CLICKED, OnBtnPasteBnClicked)
		NOTIFY_HANDLER_EX(IDC_TAB, TCN_SELCHANGE, OnTabTcnSelChange)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();
	
private:
    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT_PTR id);
	LRESULT OnBnClickedBtnEnumerate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnRead(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedChkReadAuto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBtnProcess(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTabTcnSelChange(LPNMHDR pnmh);
	LRESULT OnBtnCopyBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	LRESULT OnBtnPasteBnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl);

public:
	void SetSetting(const ProcessSetting& setting);
	void RetrieveSetting(ProcessSetting& setting);

	bool FetchProcessData(LPCVOID pTargetAddress, void* pWriteBuffer, size_t fetchSize);
	
	fastdelegate::FastDelegate1<const ProcessSetting&> m_readDelegate;
	fastdelegate::FastDelegate1<const ProcessSetting&> m_processDelegate;
	
	
private:
	CAutoCombo m_wndCmbImageName;
	CTabCtrl m_wndTab;
	boost::shared_ptr<class CSettingDialog_1D> m_pDlg1D;
	boost::shared_ptr<class CSettingDialog_2D> m_pDlg2D;
	boost::shared_ptr<class CSettingDialog_TEXT> m_pDlgTEXT;
	
	boost::shared_ptr<ProcessSetting> m_pProcessSetting_Tmp;

	void SetProcessImageNamesToComboBox();
	void ReadData();
	void ProcessData();
	void OnSelTab(size_t idx);
};


