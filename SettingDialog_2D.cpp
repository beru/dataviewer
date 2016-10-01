// This file was generated by WTL Dialog wizard 
// SettingDialog_2D.cpp : Implementation of CSettingDialog_2D

#include "stdafx.h"
#include "SettingDialog_2D.h"

#include "DataViewerCommon.h"
#include "ColorFormat.h"

#include "TwoWayMap.h"
#include "converter.h"
#include "arrayutil.h"

#include "Setting.h"

struct ColorFormatTypeMapper : public TwoWayMap<CString, ColorFormatType>
{
	ColorFormatTypeMapper()
	{
		map(CString("B5G6R5"),		ColorFormatType_B5G6R5);
		map(CString("B8G8R8"),		ColorFormatType_B8G8R8);
		map(CString("B8G8R8A8"),	ColorFormatType_B8G8R8A8);
//		map(CString("F16"),			ColorFormatType_F16);
		map(CString("F32"),			ColorFormatType_F32);
		map(CString("F64"),			ColorFormatType_F64);
		map(CString("1"),			ColorFormatType_1);
		map(CString("U8"),			ColorFormatType_U8);
		map(CString("U16"),			ColorFormatType_U16);
		map(CString("U32"),			ColorFormatType_U32);
		map(CString("S8"),			ColorFormatType_S8);
		map(CString("S16"),			ColorFormatType_S16);
		map(CString("S32"),			ColorFormatType_S32);
	}
};

ColorFormatTypeMapper colorFormatTypeMapper;

CSettingDialog_2D::CSettingDialog_2D()
{
}

CSettingDialog_2D::~CSettingDialog_2D()
{
}

BOOL CSettingDialog_2D::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

LRESULT CSettingDialog_2D::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoDataExchange();
	
	for (int i=ColorFormatType_Begin; i<=ColorFormatType_End; ++i) {
		m_wndCmbColorFormat.AddString(colorFormatTypeMapper[(ColorFormatType)i]);
	}
	m_wndRadAddressedLineFirst.Click();
	return 1;  // Let the system set the focus
}

LRESULT CSettingDialog_2D::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
//	DoDataExchange(TRUE);
//	EndDialog(wID);
	return 0;
}

LRESULT CSettingDialog_2D::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
//	EndDialog(wID);
	return 0;
}

ColorFormatType CSettingDialog_2D::getColorFormat()
{
	CString str;
	m_wndCmbColorFormat.GetLBText(m_wndCmbColorFormat.GetCurSel(), str);
	return colorFormatTypeMapper[str];
}

void CSettingDialog_2D::RetrieveSetting(std::shared_ptr<IDataSetting>& pSetting)
{
	DataSetting2D* ps = new DataSetting2D;
	pSetting = std::shared_ptr<DataSetting2D>(ps);

	CString str;
	m_wndEdtWidth.GetWindowText(ps->widthFormula, Count(ps->widthFormula));
	m_wndEdtHeight.GetWindowText(ps->heightFormula, Count(ps->heightFormula));
	if (IsDlgButtonChecked(IDC_RAD_ADDRESSEDLINE_FIRST) == BST_CHECKED) {
		ps->addressedLine = DataSetting2D::AddressedLine_First;
	}else if (IsDlgButtonChecked(IDC_RAD_ADDRESSEDLINE_LAST) == BST_CHECKED) {
		ps->addressedLine = DataSetting2D::AddressedLine_Last;
	}
	ps->colorFormat = getColorFormat();

	ps->bUsePixelStride = (IsDlgButtonChecked(IDC_CHK_PIXEL_STRIDE) == BST_CHECKED);
	m_wndEdtPixelStride.GetWindowText(ps->pixelStrideFormula, Count(ps->pixelStrideFormula));
	ps->bUseLineStride = (IsDlgButtonChecked(IDC_CHK_LINE_STRIDE) == BST_CHECKED);
	m_wndEdtLineStride.GetWindowText(ps->lineStrideFormula, Count(ps->lineStrideFormula));
	m_wndEdtMinimum.GetWindowText(ps->minimumFormula, Count(ps->minimumFormula));
	m_wndEdtMaximum.GetWindowText(ps->maximumFormula, Count(ps->maximumFormula));
}

void CSettingDialog_2D::SetSetting(const DataSetting2D& setting)
{
	CString str;
	m_wndEdtWidth.SetWindowTextW(setting.widthFormula);
	m_wndEdtHeight.SetWindowTextW(setting.heightFormula);
	switch (setting.addressedLine) {
	case DataSetting2D::AddressedLine_First:
		CheckDlgButton(IDC_RAD_ADDRESSEDLINE_FIRST, BST_CHECKED);
		CheckDlgButton(IDC_RAD_ADDRESSEDLINE_LAST, BST_UNCHECKED);
		break;
	case DataSetting2D::AddressedLine_Last:
		CheckDlgButton(IDC_RAD_ADDRESSEDLINE_FIRST, BST_UNCHECKED);
		CheckDlgButton(IDC_RAD_ADDRESSEDLINE_LAST, BST_CHECKED);
		break;
	}
	m_wndCmbColorFormat.SelectString(-1, colorFormatTypeMapper[setting.colorFormat]);
	CheckDlgButton(IDC_CHK_PIXEL_STRIDE, setting.bUsePixelStride);
	m_wndEdtPixelStride.EnableWindow(setting.bUsePixelStride);
	m_wndEdtPixelStride.SetWindowTextW(setting.pixelStrideFormula);
	CheckDlgButton(IDC_CHK_LINE_STRIDE, setting.bUseLineStride);
	m_wndEdtLineStride.EnableScrollBar(setting.bUseLineStride);
	m_wndEdtLineStride.SetWindowTextW(setting.lineStrideFormula);
	m_wndEdtMinimum.SetWindowTextW(setting.minimumFormula);
	m_wndEdtMaximum.SetWindowTextW(setting.maximumFormula);
	setMinMaxEditsEnability();
}

LRESULT CSettingDialog_2D::OnBnClickedChkPixelStride(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDC_EDT_PIXEL_STRIDE).EnableWindow(IsDlgButtonChecked(IDC_CHK_PIXEL_STRIDE) == BST_CHECKED);
	return 0;
}

LRESULT CSettingDialog_2D::OnBnClickedChkLineStride(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	GetDlgItem(IDC_EDT_LINE_STRIDE).EnableWindow(IsDlgButtonChecked(IDC_CHK_LINE_STRIDE) == BST_CHECKED);
	return 0;
}

LRESULT CSettingDialog_2D::OnBnClickedButton4(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CSettingDialog_2D::OnBnClickedButton16(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CSettingDialog_2D::OnBnClickedButton64(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

void CSettingDialog_2D::setMinMaxEditsEnability()
{
	ColorFormatType fmt = getColorFormat();
	BOOL enable = (fmt == ColorFormatType_1) ? false : IsSingleComponent(fmt);
	m_wndEdtMinimum.EnableWindow(enable);
	m_wndEdtMaximum.EnableWindow(enable);
}

LRESULT CSettingDialog_2D::OnCbnSelchangeCmbColorformat(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	setMinMaxEditsEnability();
	return 0;
}
