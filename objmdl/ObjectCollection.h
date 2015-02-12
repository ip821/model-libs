// ObjectCollection.h : Declaration of the CObjectCollection

#pragma once
#include "resource.h"       // main symbols
#include "ObjMdl_i.h"

using namespace ATL;

// CObjectCollection

class ATL_NO_VTABLE CObjectCollection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CObjectCollection, &CLSID_ObjectCollection>,
	public IObjCollection
{
public:
	CObjectCollection()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_OBJECTCOLLECTION)


	BEGIN_COM_MAP(CObjectCollection)
		COM_INTERFACE_ENTRY(IObjCollection)
		COM_INTERFACE_ENTRY(IObjArray)
	END_COM_MAP()

private:
	std::vector<CComPtr<IUnknown>> m_objects;
public:

	STDMETHOD(AddObject)(IUnknown *punk);
	STDMETHOD(InsertObject)(IUnknown *punk, UINT uiIndex);
	STDMETHOD(Clear)();
	STDMETHOD(RemoveObjectAt)(UINT uiIndex);
	STDMETHOD(IndexOf)(void* pv, UINT* puiIndex);

	STDMETHOD(GetCount)(UINT *pcObjects);
	STDMETHOD(GetAt)(UINT uiIndex, REFIID riid, void **ppv);
};

OBJECT_ENTRY_AUTO(__uuidof(ObjectCollection), CObjectCollection)
