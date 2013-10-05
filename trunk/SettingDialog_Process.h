#pragma once

#include "resource.h"       // main symbols
#include "AutoCombo.h"

struct ProcessSetting;

class CSettingDialog_Process
	: 
	public CDialogImpl<CSettingDialog_Process>,
	public CWinDataExchange<CSettingDialog_Process>,
	public CMessageFilter
{
public:
	CSettingDialog_Process();
	~CSettingDialog_Process();
	
	enum { IDD = IDD_DLG_PROCESS };
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	void GetSetting(ProcessSetting& setting);
	void SetSetting(const ProcessSetting& setting);
	
    BEGIN_MSG_MAP(CSettingDialog_Process)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_BTN_ENUMERATE, BN_CLICKED, OnBnClickedBtnEnumerate)
	END_MSG_MAP()
	
	BEGIN_DDX_MAP(CSettingDialog_Process)
		DDX_CONTROL(IDC_CMB_IMAGENAME, m_wndCmbImageName)
	END_DDX_MAP()
	
private:
	CAutoCombo m_wndCmbImageName;

	void Update();
	void SetProcessImageNamesToComboBox();
	
    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedBtnEnumerate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

