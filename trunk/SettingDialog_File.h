#pragma once

#include "resource.h"       // main symbols

struct FileSetting;

class CSettingDialog_File
	: 
	public CDialogImpl<CSettingDialog_File>,
	public CWinDataExchange<CSettingDialog_File>,
	public CMessageFilter
{
public:
	CSettingDialog_File();
	~CSettingDialog_File();
	
	enum { IDD = IDD_DLG_FILE };
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	void GetSetting(FileSetting& setting);
	void SetSetting(const FileSetting& setting);
	
    BEGIN_MSG_MAP(CSettingDialog_File)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DROPFILES, OnDropFiles)
	END_MSG_MAP()
	
	BEGIN_DDX_MAP(CSettingDialog_File)
	END_DDX_MAP()
	
private:
	void Update();
	
    // Handler prototypes:
    //  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    //  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    //  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
};

