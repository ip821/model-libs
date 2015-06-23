// Settings.cpp : Implementation of CSettings

#include "stdafx.h"
#include "RegistrySettings.h"
#include "Functions.h"

// CSettings

STDMETHODIMP CRegistrySettings::OpenSubSettings(BSTR lpszPath, ISettings** ppSettings)
{
	CHECK_E_POINTER(lpszPath);
	CHECK_E_POINTER(ppSettings);
	CString fullPath = m_strBasePath;

	{
		LPTSTR lpszFullPath = fullPath.GetBuffer(MAX_PATH);
		PathAppend(lpszFullPath, lpszPath);
		fullPath.ReleaseBuffer();
	}

	CRegKey regKey;
	if(regKey.Open(HKEY_CURRENT_USER, fullPath) == ERROR_FILE_NOT_FOUND)
	{
		auto res = regKey.Create(HKEY_CURRENT_USER, fullPath);
		if(res != ERROR_SUCCESS)
			return HRESULT_FROM_WIN32(res);
	}

	CComPtr<CRegistrySettings> pSettings;
	RETURN_IF_FAILED(CRegistrySettings::_CreatorClass::CreateInstance(NULL, IID_ISettings, (LPVOID*)&pSettings));
	pSettings->Init(fullPath, regKey.Detach());
	return pSettings->QueryInterface(IID_ISettings, (LPVOID*)ppSettings);
}

void CRegistrySettings::Init(LPCTSTR lpszBasePath, HKEY hKey)
{
	m_regKey.Attach(hKey);
	m_strBasePath = lpszBasePath;
}

STDMETHODIMP CRegistrySettings::CopyTo(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComPtr<IBstrCollection> pKeysCollection;
	RETURN_IF_FAILED(GetSubSettingsKeys(&pKeysCollection));
	UINT_PTR uiCount = 0;
	RETURN_IF_FAILED(pKeysCollection->GetCount(&uiCount));
	for (UINT_PTR i = 0; i < uiCount; i++)
	{
		CComBSTR bstrKey;
		RETURN_IF_FAILED(pKeysCollection->GetItem(i, &bstrKey));
		CComVar vValue;
		RETURN_IF_FAILED(GetVariantValue(bstrKey, &vValue));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(bstrKey, &vValue));
	}
	return S_OK;
}

STDMETHODIMP CRegistrySettings::GetVariantValue(BSTR key, VARIANT* v)
{
	CHECK_E_POINTER(key);
	CHECK_E_POINTER(v);
	DWORD dwType = 0;
	DWORD dwcbData = 0;
	auto res = m_regKey.QueryValue(key, &dwType, NULL, &dwcbData);
	if(res == ERROR_FILE_NOT_FOUND)
		return S_OK;

	if(res != ERROR_SUCCESS)
		return HRESULT_FROM_WIN32(res);

	switch(dwType)
	{
	case REG_DWORD:
		{
		DWORD dwValue = 0;
		res = m_regKey.QueryDWORDValue(key, dwValue);
		if(res != ERROR_SUCCESS)
			return HRESULT_FROM_WIN32(res);
		v->intVal = dwValue;
		v->vt = VT_I4;
		return S_OK;
		}

	case REG_SZ:
		{
		CString strValue;
		int bufferSize = dwcbData + sizeof(TCHAR);
		LPTSTR lpszBuffer = strValue.GetBuffer(bufferSize);
		ULONG ucb = bufferSize;
		res = m_regKey.QueryStringValue(key, lpszBuffer, &ucb);
		strValue.ReleaseBuffer();
		v->bstrVal = CComBSTR(strValue).Detach();
		v->vt = VT_BSTR;
		}
	}

	return S_OK;
}

STDMETHODIMP CRegistrySettings::SetVariantValue(BSTR key, VARIANT* v)
{
	CHECK_E_POINTER(key);
	CHECK_E_POINTER(v);
	switch (v->vt)
	{
	case VT_BSTR:
		return HRESULT_FROM_WIN32(m_regKey.SetStringValue(key, v->bstrVal));
	case VT_I4:
		return HRESULT_FROM_WIN32(m_regKey.SetDWORDValue(key, v->intVal));
	}

	return E_NOTIMPL;
}

STDMETHODIMP CRegistrySettings::RemoveVariantValue(BSTR key)
{
	CHECK_E_POINTER(key);
	m_regKey.DeleteValue(CString(key));
	return S_OK;
}

STDMETHODIMP CRegistrySettings::RemoveSubSettings(BSTR bstrPath)
{
	CHECK_E_POINTER(bstrPath);
	SHDeleteKey(m_regKey, CString(bstrPath));
	return S_OK;
}

STDMETHODIMP CRegistrySettings::GetSubSettingsKeys(IBstrCollection** ppArray)
{
	CHECK_E_POINTER(ppArray);
	CComPtr<IPluginManager> pPluginManager;
	RETURN_IF_FAILED(HrGetPluginManager(&pPluginManager));
	CComPtr<IBstrCollection> pObjectCollection;
	RETURN_IF_FAILED(pPluginManager->CoCreateInstance(CLSID_BstrCollection, IID_IBstrCollection, (LPVOID*)&pObjectCollection));

	DWORD dwIndex = 0;
	while(true)
	{
		DWORD dwBufferSize = 1024;
		CString strName;
		LPTSTR lpszName = strName.GetBuffer(dwBufferSize);
		FILETIME ft = {0};
		auto res = m_regKey.EnumKey(dwIndex, lpszName, &dwBufferSize, &ft);
		strName.ReleaseBuffer();
		if(res == ERROR_NO_MORE_ITEMS)
			break;

		RETURN_IF_FAILED(pObjectCollection->AddItem(CComBSTR(strName)));

		dwIndex++;
	}
	RETURN_IF_FAILED(pObjectCollection->QueryInterface(IID_IBstrCollection, (LPVOID*)ppArray));
	return S_OK;
}

STDMETHODIMP CRegistrySettings::GetCount(UINT_PTR* puiCount)
{
	UNREFERENCED_PARAMETER(puiCount);
	return E_NOTIMPL;
}

STDMETHODIMP CRegistrySettings::GetKeyByIndex(UINT_PTR uiIndex, BSTR* pbstrKey)
{
	UNREFERENCED_PARAMETER(uiIndex);
	UNREFERENCED_PARAMETER(pbstrKey);
	return E_NOTIMPL;
}
