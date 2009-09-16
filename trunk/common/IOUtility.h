#pragma once

#include <string>

namespace IOUtility
{

bool ReadFileText(LPCTSTR filePath, std::string& text);
bool ReadFileText(LPCTSTR filePath, std::wstring& text);

}	// namespace IOUItility

