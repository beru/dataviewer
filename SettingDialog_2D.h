#pragma once

#include "resource.h"       // main symbols

struct IDataSetting;
struct DataSetting2D;

class CSettingDialog_2D
	: 
	public CDialogImpl<CSettingDialog_2D>,
	public CWinDataExchange<CSettingDialog_2D>,
	public CMessageFilter
{
public:
	CSettingDialog_2D();
	~CSettingDialog_2D();
	enum { IDD = IDD_DLG_2D };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void RetrieveSetting(boost::shared_ptr<IDataSetting>& pSetting);
	void SetSetting(const DataSetting2D& setting);

    BEGIN_MSG_MAP(CSettingDialog_2D)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
        COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
	END_MSG_MAP()
	
	BEGIN_DDX_MAP(CSettingDialog_2D)
		DDX_CONTROL_HANDLE(IDC_EDT_WIDTH, m_wndEdtWidth)
		DDX_CONTROL_HANDLE(IDC_EDT_HEIGHT, m_wndEdtHeight)
		DDX_CONTROL_HANDLE(IDC_RAD_ADDRESSEDLINE_FIRST, m_wndRadAddressedLineFirst)
		DDX_CONTROL_HANDLE(IDC_RAD_ADDRESSEDLINE_LAST, m_wndRadAddressedLineLast)
		DDX_CONTROL_HANDLE(IDC_EDT_LINEOFFSET, m_wndEdtLineOffset)
		DDX_CONTROL_HANDLE(IDC_CMB_COLORFORMAT, m_wndCmbColorFormat)
	END_DDX_MAP()
	
private:
	CEdit m_wndEdtWidth;
	CEdit m_wndEdtHeight;
	CButton m_wndRadAddressedLineFirst;
	CButton m_wndRadAddressedLineLast;
	CEdit m_wndEdtLineOffset;
	CComboBox m_wndCmbColorFormat;
	
	// Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

};


