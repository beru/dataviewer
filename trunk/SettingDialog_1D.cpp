#include "stdafx.h"

#include "SettingDialog_1D.h"

#include "converter.h"
#include "Setting.h"
#include "arrayutil.h"

#include "TwoWayMap.h"

struct DataTypeMapper : public TwoWayMap<CString, DataSetting1D::DataType>
{
	DataTypeMapper()
	{
		map(CString("char"),				DataSetting1D::DataType_Char);
		map(CString("unsigned char"),		DataSetting1D::DataType_UnsignedChar);
		map(CString("short"),				DataSetting1D::DataType_Short);
		map(CString("unsigned short"),		DataSetting1D::DataType_UnsignedShort);
		map(CString("int"),					DataSetting1D::DataType_Int);
		map(CString("unsigned int"),		DataSetting1D::DataType_UnsignedInt);
		map(CString("long long"),			DataSetting1D::DataType_LongLong);
		map(CString("unsigned long long"),	DataSetting1D::DataType_UnsignedLongLong);
		map(CString("float"),				DataSetting1D::DataType_Float);
		map(CString("double"),				DataSetting1D::DataType_Double);
	}
};

DataTypeMapper dataTypeMapper;

CSettingDialog_1D::CSettingDialog_1D()
{
}

CSettingDialog_1D::~CSettingDialog_1D()
{
}

LRESULT CSettingDialog_1D::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoDataExchange();
	
	for (int i=DataSetting1D::DataType_First; i<=DataSetting1D::DataType_Last; ++i) {
		m_wndCmbType.AddString(dataTypeMapper[(DataSetting1D::DataType)i]);
	}
	m_wndCmbType.SetCurSel(0);
	
	return 1;  // Let the system set the focus
}

BOOL CSettingDialog_1D::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

void CSettingDialog_1D::SetSetting(const DataSetting1D& setting)
{
	m_wndCmbType.SelectString(-1, dataTypeMapper[setting.dataType]);
	SetDlgItemText(IDC_EDT_COUNT, setting.countFormula);
	CheckDlgButton(IDC_CHK_VIEWAUTO, (setting.viewAuto ? BST_CHECKED : BST_UNCHECKED));
	UpdateOnViewAutoCheck();
	SetDlgItemText(IDC_EDT_VIEW_MIN, setting.viewMinFormula);
	SetDlgItemText(IDC_EDT_VIEW_MAX, setting.viewMaxFormula);
}

void CSettingDialog_1D::RetreiveSetting(DataSetting1D& setting)
{
	try {
		CString str;
		m_wndCmbType.GetLBText(m_wndCmbType.GetCurSel(), str);
		setting.dataType = dataTypeMapper[str];
		GetDlgItemText(IDC_EDT_COUNT, setting.countFormula, Count(setting.countFormula));
		setting.viewAuto = (IsDlgButtonChecked(IDC_CHK_VIEWAUTO) == BST_CHECKED);
		GetDlgItemText(IDC_EDT_VIEW_MIN, setting.viewMinFormula, Count(setting.viewMinFormula));
		GetDlgItemText(IDC_EDT_VIEW_MAX, setting.viewMaxFormula, Count(setting.viewMaxFormula));
	}catch(...) {
		;
	}
	
}

void CSettingDialog_1D::RetrieveSetting(boost::shared_ptr<IDataSetting>& pSetting)
{
	DataSetting1D* p = new DataSetting1D();
	RetreiveSetting(*p);
	pSetting = boost::shared_ptr<DataSetting1D>(p);
}

LRESULT CSettingDialog_1D::OnBnClickedChkViewauto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UpdateOnViewAutoCheck();
	return 0;
}

void CSettingDialog_1D::UpdateOnViewAutoCheck()
{
	bool isAuto = (IsDlgButtonChecked(IDC_CHK_VIEWAUTO) == BST_CHECKED);
	GetDlgItem(IDC_EDT_VIEW_MIN).EnableWindow(!isAuto);
	GetDlgItem(IDC_EDT_VIEW_MAX).EnableWindow(!isAuto);
}

