// BstrCollection.h : Declaration of the CBstrCollection

#pragma once
#include "resource.h"       // main symbols
#include "ObjMdl_i.h"

using namespace ATL;


// CBstrCollection

class ATL_NO_VTABLE CBstrCollection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBstrCollection, &CLSID_BstrCollection>,
	public IBstrCollection
{
public:
	CBstrCollection()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CBstrCollection)
		COM_INTERFACE_ENTRY(IBstrCollection)
	END_COM_MAP()

private:
	std::vector<CComBSTR> m_vector;

public:

	STDMETHOD(AddItem)(BSTR bstr);
	STDMETHOD(GetCount)(UINT_PTR* pdwCount);
	STDMETHOD(GetItem)(UINT_PTR dwIndex, BSTR* pbstr);
	STDMETHOD(RemoveAt)(UINT_PTR dwIndex);
};

OBJECT_ENTRY_AUTO(__uuidof(BstrCollection), CBstrCollection)
