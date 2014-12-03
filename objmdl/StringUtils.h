#pragma once

#include <atlenc.h>
#include <vector>

static inline void StrPathAppend(CString& strPath, LPCTSTR strMore)
{
	auto lpszBuffer = strPath.GetBuffer(MAX_PATH);
	PathAppend(lpszBuffer, strMore);
	strPath.ReleaseBuffer();
}

static inline void StrPathRemoveFileName(CString& strPath)
{
	auto lpszBuffer = strPath.GetBuffer(MAX_PATH);
	PathRemoveFileSpec(lpszBuffer);
	strPath.ReleaseBuffer();
}

static inline void StrPathRemoveBackslash(CString& strPath)
{
	auto lpszBuffer = strPath.GetBuffer(MAX_PATH);
	PathRemoveBackslash(lpszBuffer);
	strPath.ReleaseBuffer();
}

static inline void StrGetTempPath(CString& strPath)
{
	auto lpszBuffer = strPath.GetBuffer(MAX_PATH);
	GetTempPath(MAX_PATH, lpszBuffer);
	strPath.ReleaseBuffer();
}

static inline HRESULT StrGuidToString(const GUID& guid, CString& strGuid)
{
	const auto bufSize = 64;
	auto lpszBuffer = strGuid.GetBuffer(bufSize);
	auto res = StringFromGUID2(guid, lpszBuffer, bufSize);
	strGuid.ReleaseBuffer();
	if (!res)
		return E_FAIL;
	return S_OK;
}

static inline void StrSplit(const CString& str, const CString& strSeparators, std::vector<CString>& result)
{
	int i = 0;
	for (CString strLine = str.Tokenize(strSeparators, i); i >= 0; strLine = str.Tokenize(strSeparators, i))
	{
		result.push_back(strLine.Trim());
	}
}

static inline CString StrQuote(const CString& str)
{
	CString strQuote;
	strQuote.Format(CString(L"\"%s\""), str);
	return strQuote;
}

static inline CString StrGetAppVersion()
{
	TCHAR lpszPath[MAX_PATH];
	if (!GetModuleFileName(NULL, lpszPath, MAX_PATH))
		return NULL;
	DWORD dwVersionSize = GetFileVersionInfoSize(lpszPath, 0);
	CTempBuffer<BYTE> pBuffer(dwVersionSize);
	GetFileVersionInfo(lpszPath, 0, dwVersionSize, pBuffer);

	UINT uLen = 0;
	VS_FIXEDFILEINFO* pFileVersion = NULL;
	VerQueryValue(pBuffer, L"\\", (LPVOID*)&pFileVersion, &uLen);
	CString strVersion;
	strVersion.Format(
		L"%u.%u.%u.%u",
		(pFileVersion->dwFileVersionMS & 0xFFFF0000) >> 16,
		(pFileVersion->dwFileVersionMS & 0x0000FFFF),
		(pFileVersion->dwFileVersionLS & 0xFFFF0000) >> 16,
		(pFileVersion->dwFileVersionLS & 0x0000FFFF)
		);
#ifdef DEBUG
	strVersion += L" Debug";
#endif
	return strVersion;
}

static inline CString StrToBase64(CString& str)
{
	auto flags = ATL_BASE64_FLAG_NOPAD | ATL_BASE64_FLAG_NOCRLF;
	int strLen = str.GetLength() * sizeof(TCHAR) + sizeof(TCHAR);
	int iLen = Base64EncodeGetRequiredLength(strLen, flags);
	CStringA result;
	{
		auto lpszBuffer = result.GetBuffer(iLen);
		Base64Encode((const LPBYTE)str.GetString(), strLen, (LPSTR)lpszBuffer, &iLen, flags);
		result.ReleaseBuffer();
	}
	USES_CONVERSION;
	return CString(CA2W(result));
}

static inline CString StrFromBase64(CString& str)
{
	int iLen = Base64DecodeGetRequiredLength(str.GetLength());
	CString result;
	{
		USES_CONVERSION;
		int strLen = iLen * sizeof(TCHAR) + sizeof(TCHAR);
		auto lpszBuffer = result.GetBuffer(strLen);
		CStringA strData = CW2A(str);
		Base64Decode((LPCSTR)strData, strData.GetLength(), (LPBYTE)lpszBuffer, &iLen);
		result.ReleaseBuffer(strLen);
	}
	return result;
}

static inline CString StrGetAppExeName()
{
	TCHAR lpszPath[MAX_PATH] = { 0 };
	auto res = GetModuleFileName(NULL, lpszPath, MAX_PATH);
	ATLASSERT(res);
	PathRemoveExtension(lpszPath);
	auto lpszName = PathFindFileName(lpszPath);
	return CString(lpszName);
}