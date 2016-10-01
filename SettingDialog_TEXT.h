#pragma once

#include "resource.h"       // main symbols

struct DataSettingTEXT;
struct IDataSetting;

class CSettingDialog_TEXT
	: 
	public CDialogImpl<CSettingDialog_TEXT>,
	public CWinDataExchange<CSettingDialog_TEXT>,
	public CMessageFilter
{
public:
	CSettingDialog_TEXT();
	~CSettingDialog_TEXT();
	
	enum { IDD = IDD_DLG_TEXT };
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void RetrieveSetting(std::shared_ptr<IDataSetting>& pSetting);
	void SetSetting(const DataSettingTEXT& setting);
	
    BEGIN_MSG_MAP(CSettingDialog_TEXT)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()
	
	BEGIN_DDX_MAP(CSettingDialog_TEXT)
	END_DDX_MAP()
	
private:
	void RetreiveSetting(DataSettingTEXT& setting);
	void Update();
	
    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

