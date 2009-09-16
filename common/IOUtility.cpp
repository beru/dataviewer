#include "stdafx.h"
#include "IOUtility.h"

#include <stdio.h>
#include <io.h>
#include <memory>

namespace IOUtility
{

bool ReadFileText(LPCTSTR filePath, std::string& text)
{
	FILE* file = _tfopen(filePath, _T("rb"));
	if (!file)
		return false;
	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	text = std::string(length, ' ');
	fread(&text[0], 1, length, file);
	fclose(file);
	return true;
}

bool ReadFileText(LPCTSTR filePath, std::wstring& text)
{
	return false;
}

}	// namespace IOUItility

