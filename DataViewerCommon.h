#pragma once

size_t AddressHexStrToNum(const CString& str);

template <typename T>
void NumToAddressHexStr(T num, CString& str)
{
	TCHAR buff[64] = {0};
	str = _itot(num, buff, 16);
}

double EvalFormula(LPCTSTR formula);

bool ReadProcessData(LPCTSTR imageName, LPCVOID pTarget, void* buffer, size_t fetchSize);
bool ReadProcessData(DWORD pid, LPCVOID pTarget, void* buffer, size_t fetchSize);

