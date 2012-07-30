#pragma once

#include "resource.h"       // main symbols
#include "AutoCombo.h"
#include "Setting.h"

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

public:
	void SetSetting(const ProcessSetting& setting, const IDataSetting* pDataSetting);
	void RetrieveSetting(ProcessSetting& setting, boost::shared_ptr<IDataSetting>& pDataSetting);

	bool FetchProcessData(LPCVOID pTargetAddress, void* pWriteBuffer, size_t fetchSize);
	
	fastdelegate::FastDelegate2<const ProcessSetting&, boost::shared_ptr<IDataSetting>& > m_readDelegate;
	fastdelegate::FastDelegate2<const ProcessSetting&, boost::shared_ptr<IDataSetting>& > m_processDelegate;

	void CopyToClipboard();
	void PasteFromClipboard();
	
private:
	CAutoCombo m_wndCmbImageName;
	CTabCtrl m_wndTab;
	boost::shared_ptr<class CSettingDialog_1D> m_pDlg1D;
	boost::shared_ptr<class CSettingDialog_2D> m_pDlg2D;
	boost::shared_ptr<class CSettingDialog_TEXT> m_pDlgTEXT;
	
	void SetProcessImageNamesToComboBox();
	void ReadData();
	void ProcessData();
	void OnSelTab(size_t idx);
};


