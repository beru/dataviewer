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

private:
	enum ColorFormatType getColorFormat();
	void setMinMaxEditsEnability();
	
    BEGIN_MSG_MAP(CSettingDialog_2D)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
        COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
		COMMAND_HANDLER(IDC_BUTTON_4, BN_CLICKED, OnBnClickedButton4)
		COMMAND_HANDLER(IDC_BUTTON_16, BN_CLICKED, OnBnClickedButton16)
		COMMAND_HANDLER(IDC_BUTTON_64, BN_CLICKED, OnBnClickedButton64)
		COMMAND_HANDLER(IDC_CHK_PIXEL_STRIDE, BN_CLICKED, OnBnClickedChkPixelStride)
		COMMAND_HANDLER(IDC_CHK_LINE_STRIDE, BN_CLICKED, OnBnClickedChkLineStride)
		COMMAND_HANDLER(IDC_CMB_COLORFORMAT, CBN_SELCHANGE, OnCbnSelchangeCmbColorformat)
	END_MSG_MAP()
	
	BEGIN_DDX_MAP(CSettingDialog_2D)
		DDX_CONTROL_HANDLE(IDC_EDT_WIDTH, m_wndEdtWidth)
		DDX_CONTROL_HANDLE(IDC_EDT_HEIGHT, m_wndEdtHeight)
		DDX_CONTROL_HANDLE(IDC_RAD_ADDRESSEDLINE_FIRST, m_wndRadAddressedLineFirst)
		DDX_CONTROL_HANDLE(IDC_RAD_ADDRESSEDLINE_LAST, m_wndRadAddressedLineLast)
		DDX_CONTROL_HANDLE(IDC_CMB_COLORFORMAT, m_wndCmbColorFormat)
		DDX_CONTROL_HANDLE(IDC_EDT_PIXEL_STRIDE, m_wndEdtPixelStride)
		DDX_CONTROL_HANDLE(IDC_EDT_LINE_STRIDE, m_wndEdtLineStride)
		DDX_CONTROL_HANDLE(IDC_EDT_MINIMUM, m_wndEdtMinimum)
		DDX_CONTROL_HANDLE(IDC_EDT_MAXIMUM, m_wndEdtMaximum)
	END_DDX_MAP()
	
	CEdit m_wndEdtWidth;
	CEdit m_wndEdtHeight;
	CButton m_wndRadAddressedLineFirst;
	CButton m_wndRadAddressedLineLast;
	CComboBox m_wndCmbColorFormat;
	CEdit m_wndEdtPixelStride;
	CEdit m_wndEdtLineStride;
	CEdit m_wndEdtMinimum;
	CEdit m_wndEdtMaximum;
	
	// Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnBnClickedChkPixelStride(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedChkLineStride(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton16(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedButton64(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelchangeCmbColorformat(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


