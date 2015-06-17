// Settings.h : Declaration of the CSettings

#pragma once
#include "resource.h"       // main symbols
#include "ObjMdl_i.h"

using namespace ATL;


// CSettings

class ATL_NO_VTABLE CRegistrySettings :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRegistrySettings, &CLSID_RegistrySettings>,
	public ISettings
{
public:
	CRegistrySettings()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SETTINGS)

	BEGIN_COM_MAP(CRegistrySettings)
		COM_INTERFACE_ENTRY(ISettings)
	END_COM_MAP()

private:
	CString m_strBasePath;
	CRegKey m_regKey;
	void Init(LPCTSTR lpszBasePath, HKEY hKey);

public:
	STDMETHOD(OpenSubSettings)(BSTR lpszPath, ISettings** ppSettings);
	STDMETHOD(GetVariantValue)(BSTR key, VARIANT* v);
	STDMETHOD(SetVariantValue)(BSTR key, VARIANT* v);
	STDMETHOD(RemoveVariantValue)(BSTR key);
	STDMETHOD(CopyTo)(IVariantObject* pVariantObject);
	STDMETHOD(GetSubSettingsKeys)(IBstrCollection** ppArray);
	STDMETHOD(RemoveSubSettings)(BSTR bstrPath);
	STDMETHOD(GetCount)(UINT_PTR* puiCount);
	STDMETHOD(GetKeyByIndex)(UINT_PTR uiIndex, BSTR* pbstrKey);
	STDMETHOD(Clear)(){ return E_NOTIMPL; };
};

OBJECT_ENTRY_AUTO(__uuidof(RegistrySettings), CRegistrySettings)
