#pragma once

#include <string>

#include "Macros.h"
#include "ManualComObjectLoader.h"
#include "objmdl_contract_i.h"
#include "CComVar.h"

static void SetCurrentDirectoryToApplicationRoot()
{
	TCHAR lpszCurrentDir[MAX_PATH] = {0};
	if (GetModuleFileName(NULL, lpszCurrentDir, MAX_PATH))
	{
		PathRemoveFileSpec(lpszCurrentDir);
		SetCurrentDirectory(lpszCurrentDir);
	}
}

extern HMODULE g_hObjMdl;

static HRESULT HrGetPluginManager(IPluginManager** ppPluginManager)
{
	CHECK_E_POINTER(ppPluginManager);
	CManualComObjectLoader loader(g_hObjMdl);
	return loader.CoCreateInstance(CLSID_PluginManager, NULL, IID_IPluginManager, (LPVOID*)ppPluginManager);
}

static IPluginManager* GetPluginManager()
{
	IPluginManager* pPluginManager;
	HrGetPluginManager(&pPluginManager);
	return pPluginManager;
}

static HRESULT HrVariantObjectGetBSTR(IVariantObject* pVariantObject, BSTR bstrKey, BSTR* pbstrValue)
{
	CHECK_E_POINTER(pVariantObject);
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(pbstrValue);

	CComVar v;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(bstrKey, &v));
	*pbstrValue = CComBSTR(v.bstrVal).Detach();
	return S_OK;
}

template <class T>
std::wstring ToHexString(T t)
{
	std::wostringstream oss;
	oss << std::hex << t;
	return oss.str();
}

static unsigned long FromHexString(std::wstring str)
{
	return std::wcstoul(str.c_str(), NULL, 16);
}

template<class Q> 
HRESULT HrCoCreateInstance(REFCLSID rclsid, Q** pp)
{
	return GetPluginManager()->CoCreateInstance(rclsid, __uuidof(Q), (LPVOID*)pp);
}

static HRESULT HrOpenSettings(BSTR bstrPath, ISettings** ppSettings)
{
	CComPtr<ISettings> pSettingsContainer;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_RegistrySettings, &pSettingsContainer));
	RETURN_IF_FAILED(pSettingsContainer->OpenSubSettings(bstrPath, ppSettings));
	return S_OK;
}

static HRESULT HrSettingsGetBSTR(ISettings* pSettings, BSTR bstrKey, BSTR* pbstrValue)
{
	CComBSTR bstrResult = L"";
	CComVar vValue;
	vValue.vt = VT_BSTR;
	if (SUCCEEDED(pSettings->GetVariantValue(bstrKey, &vValue)) && vValue.vt == VT_BSTR)
		bstrResult = vValue.bstrVal;
	*pbstrValue = bstrResult.Copy();
	return S_OK;
}

static HRESULT HrInitializeWithVariantObject(IUnknown* pObject, IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pObject);
	CHECK_E_POINTER(pVariantObject);
	CComQIPtr<IInitializeWithVariantObject> pObj = pObject;
	if (pObj)
	{
		RETURN_IF_FAILED(pObj->SetVariantObject(pVariantObject));
	}
	return S_OK;
}

static HRESULT HrGetResourceStream(HMODULE hModule, DWORD dwResourceId, LPCWSTR lpszResourceType, IStream** ppStream)
{
	CHECK_E_POINTER(ppStream);
	if (!hModule)
		return E_UNEXPECTED;

	HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(dwResourceId), lpszResourceType);
	if (!hRsrc)
		return HRESULT_FROM_WIN32(GetLastError());

	HGLOBAL hGlobal = LoadResource(hModule, hRsrc);

	if (!hGlobal)
		return HRESULT_FROM_WIN32(GetLastError());

	auto dwSizeInBytes = SizeofResource(hModule, hRsrc);
	LPVOID pvResourceData = LockResource(hGlobal);
	*ppStream = SHCreateMemStream((LPBYTE)pvResourceData, dwSizeInBytes);
	return S_OK;
}

class CCoInitializeScope
{
public:
	CCoInitializeScope()
	{
#pragma warning(suppress: 6031)
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	}

	virtual ~CCoInitializeScope()
	{
		CoUninitialize();
	}
};
