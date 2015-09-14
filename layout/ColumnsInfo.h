// ColumnRects.h : Declaration of the CColumnRects

#pragma once
#include "resource.h"       // main symbols
#include "layout_i.h"

using namespace ATL;
using namespace std;

// CColumnRects

class ATL_NO_VTABLE CColumnsInfo :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CColumnsInfo, &CLSID_ColumnsInfo>,
	public IColumnsInfo
{
public:
	CColumnsInfo()
	{
	}

	DECLARE_NO_REGISTRY()
	
	BEGIN_COM_MAP(CColumnsInfo)
		COM_INTERFACE_ENTRY(IColumnsInfo)
	END_COM_MAP()

private:
	vector<CComPtr<IColumnsInfoItem>> m_pItems;
	BOOL m_bDisabledSelection = FALSE;

public:
	STDMETHOD(AddItem)(IVariantObject* pPropsObject, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(GetItem)(UINT uiIndex, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(GetCount)(UINT* puiCount);
	STDMETHOD(Clear)();
	STDMETHOD(IsDisabledSelection)(BOOL* pbDisabled);
	STDMETHOD(DisableSelection)(BOOL bDisabled);
	STDMETHOD(FindItemIndex)(BSTR bstrName, UINT* puiIndex);
	STDMETHOD(FindItemByName)(BSTR bstrName, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(FindItemByPoint)(POINT* ppt, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(FindItemsByProperty)(BSTR bstrPropertyName, BOOL bDefined, IObjArray** ppColumnsInfoItems);
};

OBJECT_ENTRY_AUTO(__uuidof(ColumnsInfo), CColumnsInfo)
