// VariantCollection.h : Declaration of the CVariantCollection

#pragma once
#include "resource.h"       // main symbols
#include "ObjMdl_i.h"

using namespace ATL;

// CVariantCollection

class ATL_NO_VTABLE CVariantCollection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVariantCollection, &CLSID_VariantCollection>,
	public IVariantCollection
{
public:
	CVariantCollection()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_VARIANTCOLLECTION)

	BEGIN_COM_MAP(CVariantCollection)
		COM_INTERFACE_ENTRY(IVariantCollection)
	END_COM_MAP()

private:
	std::vector< CAdapt<CComVariant> > m_vector;

public:

	STDMETHOD(AddItem)(VARIANT* pv);
	STDMETHOD(GetCount)(ULONG* pulCount);
	STDMETHOD(GetItem)(ULONG ulIndex, VARIANT* pv);
	STDMETHOD(RemoveAt)(ULONG ulIndex);

};

OBJECT_ENTRY_AUTO(__uuidof(VariantCollection), CVariantCollection)
