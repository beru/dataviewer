#pragma once

#include "resource.h"       // main symbols

struct DataSetting1D;
struct IDataSetting;

class CSettingDialog_1D
	: 
	public CDialogImpl<CSettingDialog_1D>,
	public CWinDataExchange<CSettingDialog_1D>,
	public CMessageFilter
{
public:
	CSettingDialog_1D();
	~CSettingDialog_1D();
	
	enum { IDD = IDD_DLG_1D };
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void RetrieveSetting(boost::shared_ptr<IDataSetting>& pSetting);
	void SetSetting(const DataSetting1D& setting);
	
    BEGIN_MSG_MAP(CSettingDialog_1D)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_CHK_VIEWAUTO, BN_CLICKED, OnBnClickedChkViewauto)
	END_MSG_MAP()
	
	BEGIN_DDX_MAP(CSettingDialog_1D)
		DDX_CONTROL_HANDLE(IDC_CMB_TYPE, m_wndCmbType)
	END_DDX_MAP()
	
private:
	void RetreiveSetting(DataSetting1D& setting);
	void UpdateOnViewAutoCheck();

	CComboBox m_wndCmbType;

    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedChkViewauto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

