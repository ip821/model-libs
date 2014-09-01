#pragma once

#include <string>

#include "Macros.h"
#include "ManualComObjectLoader.h"
#include "objmdl_contract_i.h"

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
	CComPtr<IPluginManager> pPluginManager;
	HrGetPluginManager(&pPluginManager);
	return pPluginManager;
}

static HRESULT HrVariantObjectGetBSTR(IVariantObject* pVariantObject, BSTR bstrKey, BSTR* pbstrValue)
{
	CHECK_E_POINTER(pVariantObject);
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(pbstrValue);

	CComVariant v;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(bstrKey, &v));
	*pbstrValue = CComBSTR(v.bstrVal).Copy();
	return S_OK;
}

template <class T>
std::wstring ToHexString(T t)
{
	std::wostringstream oss;
	oss << std::hex << t;
	return oss.str();
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
	CComVariant vValue;
	vValue.vt = VT_BSTR;
	if (SUCCEEDED(pSettings->GetVariantValue(bstrKey, &vValue)) && vValue.vt == VT_BSTR)
		bstrResult = vValue.bstrVal;
	*pbstrValue = bstrResult.Copy();
	return S_OK;
}