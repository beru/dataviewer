#include "stdafx.h"
#include "winutil.h"

#include <Windowsx.h>
#include <assert.h>

// http://www.alpha-net.ne.jp/users2/uk413/vc/VCT_Clip.html
bool SetClipboardText(const wchar_t* str, size_t len)
{
	if (!str || len == 0) {
		return false;
	}
	if (!::OpenClipboard(NULL)) {
		return false;
	}
	HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, (len+1)*2);
	wcscpy((wchar_t*)hMem, str);
	::EmptyClipboard();
	::SetClipboardData(CF_UNICODETEXT, hMem);
	::CloseClipboard();
	return true;
}

bool GetClipboardText(std::wstring& str)
{
	if (!::IsClipboardFormatAvailable(CF_UNICODETEXT)) {
		return false;
	}
	if (!::OpenClipboard(NULL)) {
		return false;
	}
	HANDLE hMem = ::GetClipboardData(CF_UNICODETEXT);
	LPTSTR pMem = (LPTSTR)::GlobalLock(hMem);
	str = (const wchar_t*)pMem;
	::GlobalUnlock(hMem);
	::CloseClipboard();
	return true;
}

HBITMAP CreateDIB32(int width, int height, BITMAPINFO& bmi, void*& pBits)
{
	BITMAPINFOHEADER& header = bmi.bmiHeader;
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 32;
	header.biCompression = BI_RGB;
	header.biSizeImage = width * abs(height) * 4;
	header.biXPelsPerMeter = 0;
	header.biYPelsPerMeter = 0;
	header.biClrUsed = 0;
	header.biClrImportant = 0;

	return ::CreateDIBSection(
		(HDC)0,
		&bmi,
		DIB_RGB_COLORS,
		&pBits,
		NULL,
		0
	);
}

CString FormatMessage(LONG errorCode)
{
	TCHAR message[1024+1];
	::FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM,
		0,
		errorCode,
		0,
		message,
		1024,
		0
		);
	return message;
}

int GetCheckedRadioButton(HWND hWnd, int nIDFirstButton, int nIDLastButton)
{
	for (int id=nIDFirstButton; id<=nIDLastButton; ++id) {
		if (::IsDlgButtonChecked(hWnd, id)) {
			return id;
		}
	}
	return 0;
}

CString GetModuleFileFolderPath()
{
	CString ret;
	TCHAR szPath[_MAX_PATH+1] = {0};
	TCHAR szDrive[_MAX_DRIVE] = {0};
	TCHAR szDir[_MAX_DIR] = {0};

	// 実行モジュールのフルパスの取得
	DWORD dwRet = ::GetModuleFileName(NULL, szPath, sizeof(szPath));
	if (dwRet)
	{
		// フルパスを分解
		_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
		// szPathに実行モジュールのパスを作成
		ret += szDrive;
		ret += szDir;
	}
	return ret;
}

int GetMenuItemPos(HMENU hMenu, UINT id)
{
	int cnt = ::GetMenuItemCount(hMenu);
	for (int i=0; i<cnt; ++i) {
		if (::GetMenuItemID(hMenu, i) == id) {
			return i;
		}
	}
	return -1;
}

void SetEnableMenuItems(HMENU hMenu, bool bEnable)
{
	int cnt = ::GetMenuItemCount(hMenu);
	for (int i=0; i<cnt; ++i) {
		::EnableMenuItem(hMenu, i, MF_BYPOSITION|(bEnable ? MF_ENABLED : MF_GRAYED));
	}
}

//! 選択する値を設定
bool ComboBox_SelectItemByString(HWND hWnd, LPCTSTR value)
{
	int idx = ComboBox_FindStringExact(hWnd, -1, value);
	if (idx == CB_ERR)
		return false;
	ComboBox_SetCurSel(hWnd, idx);
	return true;
}

bool ComboBox_SelectItemByData(HWND hWnd, LPARAM data)
{
	int cnt = ComboBox_GetCount(hWnd);
	for (int i=0; i<cnt; ++i)
	{
		if (ComboBox_GetItemData(hWnd, i) == data)
		{
			ComboBox_SetCurSel(hWnd, i);
			return true;
		}
	}
	return false;
}

bool ComboBox_GetSelectedItemString(HWND hWnd, CString& value)
{
	int nSel = ComboBox_GetCurSel(hWnd);
	if (nSel == CB_ERR)
		return false;
	int len = ComboBox_GetLBTextLen(hWnd, nSel);
	TCHAR* pBuff = new TCHAR[len+1];
	if (ComboBox_GetLBText(hWnd, nSel, pBuff) == CB_ERR)
		assert(FALSE);
	value = pBuff;
	delete pBuff;
	return true;
}

LRESULT ComboBox_GetSelectedItemData(HWND hWnd)
{
	int selIdx = ComboBox_GetCurSel(hWnd);
	if (selIdx == LB_ERR)
		return LB_ERR;
	return ComboBox_GetItemData(hWnd, selIdx);
}

// http://stackoverflow.com/questions/9458555/get-dialog-size-as-defined-in-resource-file
SIZE GetDialogSize(INT nResourceId, HWND hWnd)
{

	SIZE dlgSize = {0}; 
	HINSTANCE hModule = 0;

	hModule = ::GetModuleHandle(NULL);
	HRSRC hRsrc = ::FindResource(hModule, MAKEINTRESOURCE(nResourceId), RT_DIALOG);

	HGLOBAL hTemplate = ::LoadResource(hModule, hRsrc);

	typedef ATL::_DialogSplitHelper::DLGTEMPLATEEX DLGTEMPLATEEX;
	DLGTEMPLATEEX* pTemplate = (DLGTEMPLATEEX*)::LockResource(hTemplate);

	// the approximate method of calculating
	{
		RECT rec;
		rec.left = 0;
		rec.top = 0;
		rec.right = pTemplate->cx;
		rec.bottom = pTemplate->cy;
		::MapDialogRect(hWnd, &rec);
		dlgSize.cx = rec.right - rec.left;
		dlgSize.cy = rec.bottom - rec.top;
	}
	
	UnlockResource(hTemplate);
	::FreeResource(hTemplate);
	
	return dlgSize;
}

std::basic_string<TCHAR> GetErrorMessage(DWORD id)
{
	std::basic_string<TCHAR> ret;
	void* buffer = nullptr;
	DWORD result = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		id,
		0,
		(LPWSTR)&buffer,
		0,
		NULL
	);
	if (result > 0 && buffer != nullptr) {
		ret = reinterpret_cast<TCHAR*>(buffer);
		LocalFree(buffer);
	}
	return ret;
}

