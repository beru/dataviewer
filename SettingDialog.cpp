#include "stdafx.h"
#include "SettingDialog.h"

#include "converter.h"
#include "Setting.h"
#include "arrayutil.h"

#include "SettingDialog_1D.h"
#include "SettingDialog_2D.h"
#include "SettingDialog_TEXT.h"

#include "DataViewerCommon.h"

enum TimerID
{
	TimerID_Read = 100,
};

CSettingDialog::CSettingDialog()
{
}

CSettingDialog::~CSettingDialog()
{
}

static UINT clipBoardFormat_;

LRESULT CSettingDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	clipBoardFormat_ = RegisterClipboardFormat(_T("beruponu DataViewer"));
	
	DoDataExchange();
//	SetSetting();

	CUpDownCtrl udCtrl = GetDlgItem(IDC_SPIN_ADDRESS_OFFSET_MULTIPLIER);
	udCtrl.SetRange(0, 10000);

	SetProcessImageNamesToComboBox();

	m_wndTab.InsertItem(0, _T("1D"));
	m_wndTab.InsertItem(1, _T("2D"));
	m_wndTab.InsertItem(2, _T("TEXT"));

	CRect rec;
	m_wndTab.GetClientRect(rec);
	m_wndTab.ClientToScreen(rec);
	rec.right = rec.left + 300;
	rec.bottom = rec.top + 250;

	m_pDlg1D = boost::shared_ptr<CSettingDialog_1D>(new CSettingDialog_1D);
	m_pDlg1D->Create(m_hWnd);
	m_pDlg1D->MoveWindow(rec);

	m_pDlg2D = boost::shared_ptr<CSettingDialog_2D>(new CSettingDialog_2D);
	m_pDlg2D->Create(m_hWnd);
	m_pDlg2D->MoveWindow(rec);
	m_pDlg2D->m_dataFetch_delegate.bind(this, &CSettingDialog::FetchProcessData);

	m_pDlgTEXT = boost::shared_ptr<CSettingDialog_TEXT>(new CSettingDialog_TEXT);
	m_pDlgTEXT->Create(m_hWnd);
	m_pDlgTEXT->MoveWindow(rec);
	
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	m_pDlg2D->ShowWindow(SW_HIDE);

	return 1;  // Let the system set the focus
}

BOOL CSettingDialog::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message) {
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_F5) {
			ReadData();
		}
		break;
	}
	
	if (m_wndCmbImageName.PreTranslateMessage(pMsg))
		return TRUE;
	
	if (m_pDlg1D->PreTranslateMessage(pMsg))
		return TRUE;
	
	if (m_pDlg2D->PreTranslateMessage(pMsg))
		return TRUE;

	return IsDialogMessage(pMsg);
}

BOOL CSettingDialog::OnIdle()
{
	return FALSE;
}

void CSettingDialog::SetSetting(const ProcessSetting& setting, const IDataSetting* pDataSetting)
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
	SetDlgItemText(IDC_EDT_ADDRESS_BASE, setting.addressBaseFormula);
	SetDlgItemText(IDC_EDT_ADDRESS_OFFSET, setting.addressOffsetFormula);
	convert(setting.addressOffsetMultiplier, str); SetDlgItemText(IDC_EDT_ADDRESS_OFFSET_MULTIPLIER, str);

	assert (pDataSetting);
	const type_info& dataType = typeid(*pDataSetting);
	if (dataType == typeid(DataSetting1D)) {
		m_pDlg1D->SetSetting(dynamic_cast<const DataSetting1D&>(*pDataSetting));
		m_wndTab.SetCurSel(0);
		OnSelTab(0);
	}else if (dataType == typeid(DataSetting2D)) {
		m_pDlg2D->SetSetting(dynamic_cast<const DataSetting2D&>(*pDataSetting));
		m_wndTab.SetCurSel(1);
		OnSelTab(1);
	}else {
		m_pDlgTEXT->SetSetting(dynamic_cast<const DataSettingTEXT&>(*pDataSetting));
		m_wndTab.SetCurSel(2);
		OnSelTab(2);
	}

}

void CSettingDialog::OnSelTab(size_t idx)
{
	switch (idx) {
	case 0:
		m_pDlg1D->ShowWindow(SW_SHOW);
		m_pDlg2D->ShowWindow(SW_HIDE);
		m_pDlgTEXT->ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pDlg1D->ShowWindow(SW_HIDE);
		m_pDlg2D->ShowWindow(SW_SHOW);
		m_pDlgTEXT->ShowWindow(SW_HIDE);
		break;
	case 2:
		m_pDlg1D->ShowWindow(SW_HIDE);
		m_pDlg2D->ShowWindow(SW_HIDE);
		m_pDlgTEXT->ShowWindow(SW_SHOW);
		break;
	}
}

void CSettingDialog::RetrieveSetting(ProcessSetting& setting, boost::shared_ptr<IDataSetting>& pDataSetting)
{
	try {

		CString str;

		if (IsDlgButtonChecked(IDC_RAD_IMAGENAME)) {
			setting.dataSourceKeyType = DataSourceKeyType_ImageName;
		}else if (IsDlgButtonChecked(IDC_RAD_PID)) {
			setting.dataSourceKeyType = DataSourceKeyType_PID;
		}

		m_wndCmbImageName.GetWindowText(setting.imageName, Count(setting.imageName));
		GetDlgItemText(IDC_EDT_PID, str);
		convert(str, setting.pid);
		
		GetDlgItemText(IDC_EDT_ADDRESS_BASE, setting.addressBaseFormula, Count(setting.addressBaseFormula));
		GetDlgItemText(IDC_EDT_ADDRESS_OFFSET, setting.addressOffsetFormula, Count(setting.addressOffsetFormula));
		
		GetDlgItemText(IDC_EDT_ADDRESS_OFFSET_MULTIPLIER, str);
		convert(str, setting.addressOffsetMultiplier);
		
		if (m_pDlg1D->IsWindowVisible()) {
			m_pDlg1D->RetrieveSetting(pDataSetting);
		}else if (m_pDlg2D->IsWindowVisible()) {
			m_pDlg2D->RetrieveSetting(pDataSetting);
		}else {
			m_pDlgTEXT->RetrieveSetting(pDataSetting);
		}

	}catch(...) {
		;
	}

}

LRESULT CSettingDialog::OnBnClickedBtnEnumerate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetProcessImageNamesToComboBox();
	return 0;
}

void CSettingDialog::SetProcessImageNamesToComboBox()
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


LRESULT CSettingDialog::OnTimer(UINT_PTR id)
{
	switch (id) {
	case TimerID_Read:
		ReadData();
		break;
	}
	return 0;
}

LRESULT CSettingDialog::OnTabTcnSelChange(LPNMHDR pnmh)
{
	int curSelIdx = m_wndTab.GetCurSel();
	OnSelTab(curSelIdx);
	return 0;
}

bool CSettingDialog::FetchProcessData(LPCVOID pTargetAddress, void* pWriteBuffer, size_t fetchSize)
{
	CString str;
	if (IsDlgButtonChecked(IDC_RAD_IMAGENAME)) {
		m_wndCmbImageName.GetWindowText(str);
		return ReadProcessData(str, pTargetAddress, pWriteBuffer, fetchSize);
	}else if (IsDlgButtonChecked(IDC_RAD_PID)) {
		GetDlgItemText(IDC_EDT_PID, str);
		DWORD pid = 0;
		convert(str, pid);
		return ReadProcessData(pid, pTargetAddress, pWriteBuffer, fetchSize);
	}
	return false;
}

LRESULT CSettingDialog::OnBnClickedBtnRead(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ReadData();
	return 0;
}

LRESULT CSettingDialog::OnBnClickedChkReadAuto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	bool isChecked = (IsDlgButtonChecked(IDC_CHK_READ_AUTO) == BST_CHECKED);
	if (isChecked) {
		SetTimer(TimerID_Read, 1000);
	}else {
		KillTimer(TimerID_Read);
	}
	return 0;
}

LRESULT CSettingDialog::OnBnClickedBtnProcess(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ProcessData();
	return 0;
}

void CSettingDialog::ReadData()
{
	if (m_readDelegate) {
		ProcessSetting setting;
		boost::shared_ptr<IDataSetting> pDataSetting;
		RetrieveSetting(setting, pDataSetting);
		m_readDelegate(setting, pDataSetting);
	}
}

void CSettingDialog::ProcessData()
{
	if (m_processDelegate) {
		ProcessSetting setting;
		boost::shared_ptr<IDataSetting> pDataSetting;
		RetrieveSetting(setting, pDataSetting);
		m_processDelegate(setting, pDataSetting);
	}
}

void CSettingDialog::CopyToClipboard()
{
	if (!OpenClipboard()) {
		return;
	}
	EmptyClipboard();
	HGLOBAL hG = GlobalAlloc(GHND, sizeof(ProcessSetting) + sizeof(DataSetting2D));
	uint8_t* mem = (uint8_t*) GlobalLock(hG);

	ProcessSetting processSetting;
	boost::shared_ptr<IDataSetting> pDataSetting;
	RetrieveSetting(processSetting, pDataSetting);
	memcpy(mem, &processSetting, sizeof(processSetting));
	memcpy(mem+sizeof(processSetting), pDataSetting.get(), sizeof(DataSetting2D));

	GlobalUnlock(mem);
	SetClipboardData(clipBoardFormat_, hG);
	CloseClipboard();
}

void CSettingDialog::PasteFromClipboard()
{
	if (!OpenClipboard()) {
		return;
	}
	HANDLE hClip = GetClipboardData(clipBoardFormat_);
	if (!hClip) {
		CloseClipboard();
		return;
	}
	const uint8_t* mem = (const uint8_t*) GlobalLock(hClip);
	SetSetting(*(const ProcessSetting*)mem, (const IDataSetting*)(mem+sizeof(ProcessSetting)));
	GlobalUnlock(hClip);
	CloseClipboard();
}

