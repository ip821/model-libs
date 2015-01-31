// VariantTable.h : Declaration of the CVariantTable

#pragma once
#include "resource.h"       // main symbols

#include <algorithm>
#include "BSTRSortComparer.h"
#include "LONGSortComparer.h"
#include "BSTRFilterPredicate.h"
#include "ObjMdl_i.h"

using namespace ATL;
using namespace IP::ObjectModel;

// CVariantTable

class ATL_NO_VTABLE CVariantTable :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVariantTable, &CLSID_VariantTable>,
	public IVariantTable
{
public:
	CVariantTable()
	{
	}

	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

	BEGIN_COM_MAP(CVariantTable)
		COM_INTERFACE_ENTRY(IVariantTable)
	END_COM_MAP()

private:
	std::vector<_column_data> m_columns;

	CComPtr<IObjArray> m_pColumnsArray;
	CComPtr<IObjArray> m_pObjectsArray;
	std::vector<UINT> m_indexes;
	STDMETHOD(ResetIndexes)();
	CString m_strSortColumnKey;
	CString m_strFilterColumnKey;
	CComVariant m_filterVariant;
	BOOL m_bDesc;

	STDMETHOD(RestoreSort)();
public:

	STDMETHOD(GetColumns)(IObjArray** ppObjectArray);
	STDMETHOD(SetColumns)(IObjArray* pObjectArray);
	STDMETHOD(SetObjects)(IObjArray* pObjectArray);
	STDMETHOD(GetCount)(UINT* uiCount);
	STDMETHOD(GetVariantObject)(UINT uiRowIndex, IVariantObject** ppVariantObject);
	STDMETHOD(GetVariantValue)(UINT uiRowIndex, BSTR bstrColumnKey, VARIANT* v);
	STDMETHOD(SetSort)(BSTR bstrColumnKey, BOOL bDesc);
	STDMETHOD(ResetSort)();
	STDMETHOD(SetFilter)(BSTR bstrColumnKey, VARIANT* value);
	STDMETHOD(ResetFilter)();
};

OBJECT_ENTRY_AUTO(__uuidof(VariantTable), CVariantTable)
