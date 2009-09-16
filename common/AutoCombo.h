//#include <atlcrack.h>
#include <atlstr.h>

#pragma once

// http://www.codeproject.com/wtl/wtlautocompletecombo.asp

class CAutoCombo : public CWindowImpl<CAutoCombo, CComboBox>, public CMessageFilter
{
protected:
		bool	m_bAutoComplete;
public:

	CAutoCombo()
	{
		m_bAutoComplete = true;
	}
	BEGIN_MSG_MAP(CAutoCombo)
		REFLECTED_COMMAND_CODE_HANDLER_EX(CBN_EDITUPDATE, OnEditUpdate)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	BOOL SubclassWindow( HWND hWnd )
	{
		return( CWindowImpl< CAutoCombo, CComboBox >::SubclassWindow( hWnd ) );
	}
	

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnEditUpdate(UINT , int, HWND)
	{
		if (!m_bAutoComplete) 
			return 0;

		// Get the text in the edit box
		CString str;
		GetWindowText(str);
		int nLength = str.GetLength();
		  
		// Currently selected range
		DWORD dwCurSel = GetEditSel();
		WORD dStart = LOWORD(dwCurSel);
		WORD dEnd   = HIWORD(dwCurSel);

		// Search for, and select in, and string in the combo box that is prefixed
		// by the text in the edit box
		if (SelectString(-1, str) == CB_ERR)
		{
			SetWindowText(str);		// No text selected, so restore what was there before
			if (dwCurSel != CB_ERR)
				SetEditSel(dStart, dEnd);	//restore cursor postion
		}

		// Set the text selection as the additional text that we have added
		if (dEnd < nLength && dwCurSel != CB_ERR)
			SetEditSel(dStart, dEnd);
		else
			SetEditSel(nLength, -1);
		return 0;
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		// Need to check for backspace/delete. These will modify the text in
		// the edit box, causing the auto complete to just add back the text
		// the user has just tried to delete. 

		if (pMsg->message == WM_KEYDOWN)
		{
			m_bAutoComplete = true;

			int nVirtKey = (int) pMsg->wParam;
			if (nVirtKey == VK_DELETE || nVirtKey == VK_BACK)
				m_bAutoComplete = false;
		}

		return FALSE;
	}

};

