// VariantObject.h : Declaration of the CVariantObject

#pragma once
#include "resource.h"       // main symbols
#include "ObjMdl_i.h"

using namespace ATL;


// CVariantObject

class ATL_NO_VTABLE CVariantObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVariantObject, &CLSID_VariantObject>,
	public IVariantObject
{
public:
	CVariantObject()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_VARIANTOBJECT)

	BEGIN_COM_MAP(CVariantObject)
		COM_INTERFACE_ENTRY(IVariantObject)
	END_COM_MAP()

private:
	std::map<CString, CComVar> m_values;
public:

	STDMETHOD(Clear)();
	STDMETHOD(GetVariantValue)(BSTR key, VARIANT* v);
	STDMETHOD(SetVariantValue)(BSTR key, VARIANT* v);
	STDMETHOD(RemoveVariantValue)(BSTR key);
	STDMETHOD(CopyTo)(IVariantObject* pVariantObject);
	STDMETHOD(GetCount)(UINT_PTR* puiCount);
	STDMETHOD(GetKeyByIndex)(UINT_PTR uiIndex, BSTR* pbstrKey);
};

OBJECT_ENTRY_AUTO(__uuidof(VariantObject), CVariantObject)
