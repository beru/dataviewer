#pragma once

#include <string>
#include <sstream>
#include <algorithm>

typedef std::basic_string<TCHAR> tstring;

//! str 中の文字を separatorで分断、結果はstrsに格納する
template <typename Container, typename T>
Container& Split(
	const std::basic_string<T>& str,
	const std::basic_string<T>& separator,
	Container& strs)
{
	strs.clear();
	size_t foundPos = 0;
	size_t searchPos = 0;
	while ((foundPos = str.find(separator, searchPos)) != std::basic_string<T>::npos)
	{
		std::basic_string<T> s = str.substr( searchPos, (foundPos - searchPos) );
		strs.push_back(s);
		searchPos = foundPos + separator.size();
	}
	strs.push_back(
		str.substr( searchPos, str.size() - searchPos )
	);
	return strs;
}

//! CStringが引数版、MFC配慮
template <typename Container>
Container& Split(const CString& str, const CString& separator, Container& strs)
{
	std::basic_string<TCHAR> stlStr = ((CString&)str).GetBuffer(0);
	std::basic_string<TCHAR> stlSep = ((CString&)separator).GetBuffer(0);
	return Split(stlStr, stlSep, strs);
}

//! str 中の文字を separatorで分断、結果はstrsに格納する
template <typename T, typename Container>
Container& SplitSplit(
	const std::basic_string<T>& str,
	const std::basic_string<T>& separator,
	const std::basic_string<T>& secondSeparator,
	Container& strVecVec)
{
	std::vector<std::basic_string<T> > vec;
	std::vector<std::basic_string<T> > temp;
	split(str, separator, vec);
	for (size_t i=0; i<vec.size(); ++i) {
		split(vec[i], secondSeparator, temp);
		strVecVec.push_back(temp);
	}
	return strVecVec;
}

//! 文字列中の文字列を他の文字列に置換
template <typename T>
std::basic_string<T>& Replace(
	std::basic_string<T>& str,
	const std::basic_string<T>& sb,
	const std::basic_string<T>& sa)
{
	size_t n, nb = 0;
	while ((n = str.find(sb,nb)) != std::string::npos)
	{
		str.replace(n,sb.size(),sa);
		nb = n + sa.size();
	}
	return str;
}

//! 指定した文字列で結合する
template<typename Container, typename T>
std::basic_string<T>& Join(
	const Container& strs,
	const std::basic_string<T>& separator,
	std::basic_string<T>& ret)
{
	int i;
	int size = (int)strs.size();
	if (size > 0) {
		for (i=0; i<size-1; ++i) {
			ret += strs[i] + separator;
		}
		ret += strs[i];
	}
	return ret;
}

//! 'で囲む。SQL用文字列処理
tstring QuoteForSQL(const CString& src);

//! 'で囲む。SQL用文字列処理
inline std::string QuoteForSQL(const std::string& src)
{
	std::string tmp = src;
	tmp = Replace(tmp, std::string("\'"), std::string("\'\'"));
	tmp = "\'" + tmp;
	tmp += "\'";
	return tmp;
}
inline std::wstring QuoteForSQL(const std::wstring& src)
{
	std::wstring tmp = src;
	tmp = Replace(tmp, std::wstring(L"\'"), std::wstring(L"\'\'"));
	tmp = L"\'" + tmp;
	tmp += L"\'";
	return tmp;
}

TCHAR* RemoveTrailLetter(TCHAR* buf, TCHAR removeLetter);

TCHAR* RemoveTrailZero(TCHAR* buf);

CString& RemoveTrailZero(CString& str);

/* TODO: 可変長書式に対応。。。
TCHAR* FormatNumber(TCHAR* buf, const TCHAR* format, bool removeTrailZero = true);

CString& FormatNumber(CString& str, const TCHAR* format, bool removeTrailZero = true);
*/

std::string SjisToUtf8(LPCTSTR sjis);
std::string Utf8ToSjis(LPCSTR utf8);
