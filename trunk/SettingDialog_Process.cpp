#include "stdafx.h"

#include "SettingDialog_Process.h"

#include "Setting.h"
#include "converter.h"
#include "arrayutil.h"

CSettingDialog_Process::CSettingDialog_Process()
{
}

CSettingDialog_Process::~CSettingDialog_Process()
{
}

LRESULT CSettingDialog_Process::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoDataExchange();

	SetProcessImageNamesToComboBox();
	
	return 1;  // Let the system set the focus
}

BOOL CSettingDialog_Process::PreTranslateMessage(MSG* pMsg)
{
	if (m_wndCmbImageName.PreTranslateMessage(pMsg))
		return TRUE;
	return IsDialogMessage(pMsg);
}

void CSettingDialog_Process::SetSetting(const ProcessSetting& setting)
{
	switch (setting.dataSourceKeyType) {
	case DataSourceKeyType_ImageName:
		CheckDlgButton(IDC_RAD_IMAGENAME, BST_CHECKED);
		CheckDlgButton(IDC_RAD_PID, BST_UNCHECKED);
		break;
	case DataSourceKeyType_PID:
		CheckDlgButton(IDC_RAD_IMAGENAME, BST_UNCHECKED);
		CheckDlgButton(IDC_RAD_PID, BST_CHECKED);
		break;
	}
	
	CString str;
	m_wndCmbImageName.SetWindowText(setting.imageName);
	convert(setting.pid, str); SetDlgItemText(IDC_EDT_PID, str);

}

void CSettingDialog_Process::GetSetting(ProcessSetting& setting)
{
	if (IsDlgButtonChecked(IDC_RAD_IMAGENAME)) {
		setting.dataSourceKeyType = DataSourceKeyType_ImageName;
	}else if (IsDlgButtonChecked(IDC_RAD_PID)) {
		setting.dataSourceKeyType = DataSourceKeyType_PID;
	}

	m_wndCmbImageName.GetWindowText(setting.imageName, Count(setting.imageName));
	CString str;
	GetDlgItemText(IDC_EDT_PID, str);
	convert(str, setting.pid);
}

void CSettingDialog_Process::SetProcessImageNamesToComboBox()
{
	CString strSetted;
	m_wndCmbImageName.GetWindowText(strSetted);
	
	m_wndCmbImageName.LockWindowUpdate(TRUE);
	m_wndCmbImageName.ResetContent();
	const size_t ARRAY_SIZE = 1024;
	std::vector<DWORD> processIDs(ARRAY_SIZE);
	DWORD bytesReturned = 0;
	BOOL ret = EnumProcesses(&processIDs[0], ARRAY_SIZE*sizeof(DWORD), &bytesReturned);
	size_t cnt = bytesReturned / sizeof(DWORD);

	TCHAR filePath[_MAX_PATH+1];
	for (size_t i=0; i<cnt; ++i) {
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processIDs[i]);
		if (hProcess != NULL) {
			HMODULE hModule;
			DWORD cbNeeded;
			if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
				DWORD ret = GetModuleFileNameEx(hProcess, hModule, filePath, _MAX_PATH);
				if (ret) {
					m_wndCmbImageName.AddString(PathUtility::FindFileName(filePath));
				}
			}
			CloseHandle(hProcess);
		}
	}
	m_wndCmbImageName.SetWindowText(strSetted);
	m_wndCmbImageName.LockWindowUpdate(FALSE);
}

LRESULT CSettingDialog_Process::OnBnClickedBtnEnumerate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetProcessImageNamesToComboBox();
	return 0;
}

