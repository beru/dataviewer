#include "stdafx.h"
#include "DataViewerCommon.h"

#include "muParser/muParser.h"
#include "converter.h"

size_t AddressHexStrToNum(const CString& str)
{
	CString wrk = str;
	if (str.Left(2).MakeLower() == "0x") {
		wrk = str.Right(str.GetLength() - 2);
	}
	TCHAR* endPtr;
	return _tcstoul(str, &endPtr, 16);
}

double EvalFormula(LPCTSTR formula)
{
    mu::Parser parser;
	if (_tcslen(formula) == 0)
		parser.SetExpr(L"0");
	else {
		parser.SetExpr(formula);
	}
	try {
		return parser.Eval();
	}catch (...) {
		return 0;
	}
}

bool ReadProcessData(HANDLE hProcess, LPCVOID pTarget, void* buffer, size_t fetchSize)
{
	if (hProcess == NULL)
		return false;
	SIZE_T nBytesRead;
	BOOL ret = ReadProcessMemory(hProcess, pTarget, buffer, fetchSize, &nBytesRead);
	CloseHandle(hProcess);
	return ret != FALSE;
}

HANDLE OpenProcessHandleByName(LPCTSTR name)
{
	const size_t ARRAY_SIZE = 1024;
	std::vector<DWORD> processIDs(ARRAY_SIZE);
	DWORD bytesReturned = 0;
	BOOL ret = EnumProcesses(&processIDs[0], ARRAY_SIZE*sizeof(DWORD), &bytesReturned);
	size_t cnt = bytesReturned / sizeof(DWORD);

	for (size_t i=0; i<cnt; ++i) {
		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processIDs[i]);
		if (hProcess != NULL) {
			HMODULE hModule;
			DWORD cbNeeded;
			if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
				TCHAR filePath[_MAX_PATH+1];
				DWORD ret = GetModuleFileNameEx(hProcess, hModule, filePath, _MAX_PATH);
				if (ret) {
					if (PathUtility::FindFileName(filePath).CompareNoCase(name) == 0) {
						return hProcess;
					}
				}
			}
			CloseHandle(hProcess);
		}
	}
	return NULL;
}

bool ReadProcessData(LPCTSTR imageName, LPCVOID pTarget, void* buffer, size_t fetchSize)
{
	return ReadProcessData(OpenProcessHandleByName(imageName), pTarget, buffer, fetchSize);
}


bool ReadProcessData(DWORD pid, LPCVOID pTarget, void* buffer, size_t fetchSize)
{
	return ReadProcessData(OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid), pTarget, buffer, fetchSize);
}


