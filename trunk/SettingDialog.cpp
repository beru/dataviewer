#include "stdafx.h"
#include "SettingDialog.h"

#include "converter.h"
#include "Setting.h"
#include "arrayutil.h"

#include "SettingDialog_Process.h"
#include "SettingDialog_File.h"

#include "SettingDialog_1D.h"
#include "SettingDialog_2D.h"
#include "SettingDialog_TEXT.h"

#include "DataViewerCommon.h"
#include "winutil.h"

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
	
	m_wndTabSrc.InsertItem(0, _T("Process"));
	m_wndTabSrc.InsertItem(1, _T("File"));
	
	m_wndTab.InsertItem(0, _T("1D"));
	m_wndTab.InsertItem(1, _T("2D"));
	m_wndTab.InsertItem(2, _T("TEXT"));
	
	SIZE sz = GetDialogSize(IDD_DLG_SETTING, m_hWnd);
	CRect rec;
	m_wndTabSrc.GetClientRect(rec);
	m_wndTabSrc.ClientToScreen(rec);
	rec.right = rec.left + sz.cx * 0.95;
	rec.bottom = rec.top + sz.cy * 0.14;

	m_pDlgProcess = boost::shared_ptr<CSettingDialog_Process>(new CSettingDialog_Process);
	m_pDlgProcess->Create(m_hWnd);
	m_pDlgProcess->MoveWindow(rec);

	m_pDlgFile = boost::shared_ptr<CSettingDialog_File>(new CSettingDialog_File);
	m_pDlgFile->Create(m_hWnd);
	m_pDlgFile->MoveWindow(rec);

	m_wndTab.GetClientRect(rec);
	m_wndTab.ClientToScreen(rec);
	rec.right = rec.left + sz.cx * 0.95;
	rec.bottom = rec.top + sz.cy * 0.5;
	m_pDlg1D = boost::shared_ptr<CSettingDialog_1D>(new CSettingDialog_1D);
	m_pDlg1D->Create(m_hWnd);
	m_pDlg1D->MoveWindow(rec);

	m_pDlg2D = boost::shared_ptr<CSettingDialog_2D>(new CSettingDialog_2D);
	m_pDlg2D->Create(m_hWnd);
	m_pDlg2D->MoveWindow(rec);

	m_pDlgTEXT = boost::shared_ptr<CSettingDialog_TEXT>(new CSettingDialog_TEXT);
	m_pDlgTEXT->Create(m_hWnd);
	m_pDlgTEXT->MoveWindow(rec);
	
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	m_pDlgProcess->ShowWindow(SW_SHOW);
	m_pDlg2D->ShowWindow(SW_SHOW);
	m_wndTab.SetCurSel(1);

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

	if (m_pDlgProcess->PreTranslateMessage(pMsg))
		return TRUE;
	
	if (m_pDlgFile->PreTranslateMessage(pMsg))
		return TRUE;

	if (m_pDlg1D->PreTranslateMessage(pMsg))
		return TRUE;
	
	if (m_pDlg2D->PreTranslateMessage(pMsg))
		return TRUE;

	if (m_pDlgTEXT->PreTranslateMessage(pMsg))
		return TRUE;

	return IsDialogMessage(pMsg);
}

BOOL CSettingDialog::OnIdle()
{
	return FALSE;
}

void CSettingDialog::SetSetting(const SourceSetting* pSrcSetting, const IDataSetting* pDataSetting)
{
	assert (pSrcSetting);
	{
		const type_info& dataType = typeid(*pSrcSetting);
		if (dataType == typeid(ProcessSetting)) {
			m_pDlgProcess->SetSetting((const ProcessSetting&)*pSrcSetting);
			m_wndTabSrc.SetCurSel(0);
			OnSelTabSrc(0);
		}else if (dataType == typeid(FileSetting)) {
			m_pDlgFile->SetSetting((const FileSetting&)*pSrcSetting);
			m_wndTabSrc.SetCurSel(1);
			OnSelTabSrc(1);
		}
	}

	CString str;
	SetDlgItemText(IDC_EDT_ADDRESS_BASE, pSrcSetting->addressBaseFormula);
	SetDlgItemText(IDC_EDT_ADDRESS_OFFSET, pSrcSetting->addressOffsetFormula);
	convert(pSrcSetting->addressOffsetMultiplier, str); SetDlgItemText(IDC_EDT_ADDRESS_OFFSET_MULTIPLIER, str);
	
	assert (pDataSetting);
	{
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

}

void CSettingDialog::OnSelTabSrc(size_t idx)
{
	switch (idx) {
	case 0:
		m_pDlgProcess->ShowWindow(SW_SHOW);
		m_pDlgFile->ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pDlgProcess->ShowWindow(SW_HIDE);
		m_pDlgFile->ShowWindow(SW_SHOW);
		break;
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

void CSettingDialog::RetrieveSetting(
	boost::shared_ptr<SourceSetting>& pSrcSetting,
	boost::shared_ptr<IDataSetting>& pDataSetting)
{
	try {
		CString str;
		if (m_pDlgProcess->IsWindowVisible()) {
			ProcessSetting* p = new ProcessSetting();
			m_pDlgProcess->GetSetting(*p);
			pSrcSetting = boost::shared_ptr<SourceSetting>(p);
		}else if (m_pDlgFile->IsWindowVisible()) {
			FileSetting* p = new FileSetting();
			m_pDlgFile->GetSetting(*p);
			pSrcSetting = boost::shared_ptr<SourceSetting>(p);
		}
		SourceSetting& setting = *pSrcSetting;
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

LRESULT CSettingDialog::OnTimer(UINT_PTR id)
{
	switch (id) {
	case TimerID_Read:
		ReadData();
		break;
	}
	return 0;
}

LRESULT CSettingDialog::OnTabSrcTcnSelChange(LPNMHDR pnmh)
{
	int curSelIdx = m_wndTabSrc.GetCurSel();
	OnSelTabSrc(curSelIdx);
	return 0;
}


LRESULT CSettingDialog::OnTabTcnSelChange(LPNMHDR pnmh)
{
	int curSelIdx = m_wndTab.GetCurSel();
	OnSelTab(curSelIdx);
	return 0;
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
		boost::shared_ptr<SourceSetting> pSrcSetting;
		boost::shared_ptr<IDataSetting> pDataSetting;
		RetrieveSetting(pSrcSetting, pDataSetting);
		m_readDelegate(pSrcSetting, pDataSetting);
	}
}

void CSettingDialog::ProcessData()
{
	if (m_processDelegate) {
		boost::shared_ptr<SourceSetting> pSrcSetting;
		boost::shared_ptr<IDataSetting> pDataSetting;
		RetrieveSetting(pSrcSetting, pDataSetting);
		m_processDelegate(pSrcSetting, pDataSetting);
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

	//boost::shared_ptr<SourceSetting> pSourceSetting;
	//boost::shared_ptr<IDataSetting> pDataSetting;
	//RetrieveSetting(pSourceSetting, pDataSetting);
	//memcpy(mem, pSourceSetting.get(), sizeof(processSetting));
	//memcpy(mem+sizeof(processSetting), pDataSetting.get(), sizeof(DataSetting2D));

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
	//SetSetting(*(const ProcessSetting*)mem, (const IDataSetting*)(mem+sizeof(ProcessSetting)));
	GlobalUnlock(hClip);
	CloseClipboard();
}

