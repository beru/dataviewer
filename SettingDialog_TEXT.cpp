#include "stdafx.h"

#include "SettingDialog_TEXT.h"

#include "converter.h"
#include "Setting.h"
#include "arrayutil.h"

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
		GetDlgItemText(IDC_EDT_BYTES, setting.bytesFormula, Count(setting.bytesFormula));
	}catch(...) {
		;
	}
	
}

void CSettingDialog_TEXT::RetrieveSetting(boost::shared_ptr<IDataSetting>& pSetting)
{
	DataSettingTEXT* ps = new DataSettingTEXT();
	RetreiveSetting(*ps);
	pSetting = boost::shared_ptr<DataSettingTEXT>(ps);
}

