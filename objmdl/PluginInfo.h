#pragma once

#include "ObjMdl_i.h"

using namespace ATL;

class ATL_NO_VTABLE CPluginInfo : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPluginInfo, &CLSID_PluginInfo>,
	public IPluginInfo
{
protected:
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<IPluginManager> m_pPluginManager;

public:
	CPluginInfo()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CPluginInfo)
		COM_INTERFACE_ENTRY(IPluginInfo)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IVariantObject, m_pVariantObject)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT();

	HRESULT FinalConstruct();

	void FinalRelease()
	{
	}

private:
	STDMETHOD(GetStringValue)(BSTR bstrKey, BSTR* bstr);
public:
	STDMETHOD(GetName)(BSTR* bstrName);
	STDMETHOD(GetDescription)(BSTR* bstrName);
	STDMETHOD(GetNamespace)(GUID* guidNamespace);
	STDMETHOD(GetType)(GUID* guidType);
	STDMETHOD(GetId)(GUID* guidId);
	STDMETHOD(GetCLSID)(GUID* guidId);
	STDMETHOD(CreateInstance)(IUnknown** ppUnknown);
};

OBJECT_ENTRY_AUTO(__uuidof(PluginInfo), CPluginInfo)
