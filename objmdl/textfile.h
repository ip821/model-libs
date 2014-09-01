#pragma once

#include <fstream>

class CTextFile
{
public:

	static void ReadAllText(LPCTSTR lpszPath, CString& strText)
	{
		std::wfstream fs(lpszPath, std::ios::in | std::ios::binary);
		const auto bufSize = 1024;
		TCHAR buffer[bufSize] = {0};
		while (auto cbRead = fs.rdbuf()->sgetn(buffer, bufSize))
		{
			strText.Append(buffer, (int)cbRead);
		}
		fs.close();
	}

	static void WriteAllText(LPCTSTR lpszPath, const CString& strText)
	{
		std::wfstream fs(lpszPath, std::ios::out | std::ios::binary | std::ios::trunc);
		fs.write(strText, strText.GetLength());
		fs.close();
	}
};