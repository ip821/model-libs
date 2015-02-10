#pragma once

#include <fstream>
#include <Objidl.h>

class CTextFile
{
public:

	static void ReadAllTextFromStream(IStream* pStream, CString& strText)
	{
		USES_CONVERSION;
		const auto bufSize = 1024;
		while (true)
		{
			char buffer[bufSize] = { 0 };
			ULONG cbRead = 0;
			pStream->Read(&buffer, bufSize, &cbRead);
			if (!cbRead)
				break;
			strText.Append(CA2W(buffer), cbRead);
		}
	}

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