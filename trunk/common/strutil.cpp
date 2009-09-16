#include "stdafx.h"
#include "strutil.h"
// #include "UnicodeF.h"

tstring QuoteForSQL(const CString& src)
{
	CString tmp = src;
	tmp.Replace(_T("\'"), _T("\'\'"));
	tmp = _T("\'") + tmp;
	tmp += _T("\'");
	tstring ret = tmp.GetBuffer(0);
	return ret;
}

TCHAR* RemoveTrailLetter(TCHAR* buf, TCHAR removeLetter)
{
	int len = _tcslen(buf) - 1;
	int i;
	for (i=len; i>0; --i)
	{
		if (buf[i] != removeLetter)
			break;
	}
	if (i >= 0)
		buf[i+1] = NULL;
	return buf;
}

//! 末尾の小数点の零を消す。.の次の0で終わっている場合は勘弁
TCHAR* RemoveTrailZero(TCHAR* buf)
{
	int size = _tcslen(buf) - 1;
	if (size <= 1)
		return buf;
	TCHAR* dotPos = std::find(buf, buf+size, _T('.'));
	if (dotPos >= buf+size)
		return buf;
	else
	{
		RemoveTrailLetter(dotPos+1, _T('0'));
		return buf;
	}
}

CString& RemoveTrailZero(CString& str)
{
	RemoveTrailZero(str.GetBuffer(0));
	str.ReleaseBuffer();
	return str;
}

/* TODO: 可変長書式に対応。。。
TCHAR* FormatNumber(TCHAR* buf, const TCHAR* format, bool removeTrailZero)
{
	_stprintf(buf, format);
	if (removeTrailZero)
		RemoveTrailZero(buf);
	return buf;
}

CString& FormatNumber(CString& str, const TCHAR* format, bool removeTrailZero)
{
	ftos(str.GetBuffer(0));
	str.ReleaseBuffer();
	return str;
}
*/
/*
std::string SjisToUtf8(LPCSTR sjis)
{
	int nBytesOut = 0;
	char* pChar = CUnicodeF::sjis_to_utf8(sjis, &nBytesOut);
	std::string tmp = pChar;
	delete pChar;
	return tmp;
}

std::string Utf8ToSjis(LPCSTR utf8)
{
	int nBytesOut = 0;
	char* pChar = CUnicodeF::utf8_to_sjis(utf8, &nBytesOut);
	std::string tmp = pChar;
	delete pChar;
	return tmp;
}

*/