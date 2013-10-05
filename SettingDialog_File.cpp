#include "stdafx.h"

#include "SettingDialog_File.h"

#include "Setting.h"

CSettingDialog_File::CSettingDialog_File()
{
}

CSettingDialog_File::~CSettingDialog_File()
{
}

LRESULT CSettingDialog_File::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoDataExchange();

	DragAcceptFiles();
	
	return 1;  // Let the system set the focus
}

LRESULT CSettingDialog_File::OnDropFiles(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	UINT	i;
	UINT	nSize;
	UINT	ret;
	HDROP	hDrop;
	TCHAR	buff[1024];
	
	hDrop = (HDROP)wParam;
	nSize = ::DragQueryFile(hDrop,-1,NULL,0);
	if (nSize) {
		ret = ::DragQueryFile(hDrop, 0, buff, sizeof(buff)/2);
		SetDlgItemText(IDC_EDT_FILEPATH, buff);
	}
	::DragFinish(hDrop);
	return	0;
}

BOOL CSettingDialog_File::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

void CSettingDialog_File::SetSetting(const FileSetting& setting)
{
	SetDlgItemText(IDC_EDT_FILEPATH, setting.filePath);
}

void CSettingDialog_File::GetSetting(FileSetting& setting)
{
	GetDlgItemText(IDC_EDT_FILEPATH, setting.filePath, sizeof(setting.filePath)*2);
}

