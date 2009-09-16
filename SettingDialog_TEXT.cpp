#include "stdafx.h"

#include "SettingDialog_TEXT.h"

#include "converter.h"
#include "Setting.h"

CSettingDialog_TEXT::CSettingDialog_TEXT()
{
}

CSettingDialog_TEXT::~CSettingDialog_TEXT()
{
}

LRESULT CSettingDialog_TEXT::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DoDataExchange();
	
	return 1;  // Let the system set the focus
}

BOOL CSettingDialog_TEXT::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

void CSettingDialog_TEXT::SetSetting(const DataSettingTEXT& setting)
{
	SetDlgItemText(IDC_EDT_BYTES, setting.bytesFormula);
}

void CSettingDialog_TEXT::RetreiveSetting(DataSettingTEXT& setting)
{
	try {
		CString str;
		GetDlgItemText(IDC_EDT_BYTES, setting.bytesFormula);
	}catch(...) {
		;
	}
	
}

void CSettingDialog_TEXT::RetrieveSetting(boost::shared_ptr<IDataSetting>& pSetting)
{
	boost::shared_ptr<DataSettingTEXT> ps = boost::shared_ptr<DataSettingTEXT>(new DataSettingTEXT);
	RetreiveSetting(*ps);
	pSetting = ps;
}

