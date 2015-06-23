// VariantTable.cpp : Implementation of CVariantTable

#include "stdafx.h"
#include "VariantTable.h"


// CVariantTable

HRESULT CVariantTable::FinalConstruct()
{
	return S_OK;
}

void CVariantTable::FinalRelease()
{
}

STDMETHODIMP CVariantTable::GetColumns(IObjArray** ppObjectArray)
{
	CHECK_E_POINTER(ppObjectArray);
	return m_pColumnsArray->QueryInterface(IID_IObjArray, (LPVOID*)ppObjectArray);
}

STDMETHODIMP CVariantTable::SetColumns(IObjArray* pObjectArray)
{
	CHECK_E_POINTER(pObjectArray);
	m_pColumnsArray = pObjectArray;
	m_columns.clear();

	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for(UINT i = 0; i < uiCount; i++)
	{
		_column_data column_data = {0};
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, IID_IVariantObject, (LPVOID*)&pVariantObject));
		CComVar vKey;
		pVariantObject->GetVariantValue(Metadata::Table::Column::Key, &vKey);
		if(vKey.vt == VT_BSTR)
		{
			column_data.strColumnKey = vKey.bstrVal;
		}

		CComVar vName;
		pVariantObject->GetVariantValue(Metadata::Table::Column::Name, &vName);
		if(vName.vt == VT_BSTR)
		{
			column_data.strColumnName = vName.bstrVal;
		}

		CComVar vType;
		pVariantObject->GetVariantValue(Metadata::Table::Column::Type, &vType);
		if(vType.vt == VT_UI2)
		{
			column_data.vt = (VARENUM)vType.uiVal;
		}
		m_columns.push_back(column_data);
	}
	return S_OK;
}

STDMETHODIMP CVariantTable::SetObjects(IObjArray* pObjectArray)
{
	CHECK_E_POINTER(pObjectArray);
	m_pObjectsArray = pObjectArray;
	ResetIndexes();
	return S_OK;
}

STDMETHODIMP CVariantTable::GetCount(UINT* uiCount)
{
	CHECK_E_POINTER(uiCount);
	*uiCount = static_cast<UINT>(m_indexes.size());
	return S_OK;
}

STDMETHODIMP CVariantTable::GetVariantObject(UINT uiRowIndex, IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(ppVariantObject);
	uiRowIndex = m_indexes[uiRowIndex];
	CComPtr<IVariantObject> pVariantObject;
	RETURN_IF_FAILED(m_pObjectsArray->GetAt(uiRowIndex, IID_IVariantObject, (LPVOID*)&pVariantObject));
	return pVariantObject->QueryInterface(IID_IVariantObject, (LPVOID*)ppVariantObject);
}

STDMETHODIMP CVariantTable::GetVariantValue(UINT uiRowIndex, BSTR bstrColumnKey, VARIANT* v)
{
	CHECK_E_POINTER(bstrColumnKey);
	CHECK_E_POINTER(v);
	CComPtr<IVariantObject> pVariantObject;
	RETURN_IF_FAILED(GetVariantObject(uiRowIndex, &pVariantObject));
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(bstrColumnKey, v));
	return S_OK;
}

STDMETHODIMP CVariantTable::ResetIndexes()
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pObjectsArray->GetCount(&uiCount));
	m_indexes.clear();
	m_indexes.resize(uiCount);
	for(UINT i = 0; i < uiCount; i++)
	{
		m_indexes[i] = i;
	}
	return S_OK;
}

STDMETHODIMP CVariantTable::SetSort(BSTR bstrColumnKey, BOOL bDesc)
{
	CHECK_E_POINTER(bstrColumnKey);
	m_strSortColumnKey = bstrColumnKey;
	m_bDesc = bDesc;
	auto vt = VT_BSTR;

	auto it = std::find_if(
		m_columns.begin(),
		m_columns.end(),
		[&](_column_data column_data)
	{
		return column_data.strColumnKey == bstrColumnKey;
	}
	);

	if(it != m_columns.end())
	{
		vt = it->vt;
	}

	switch(vt)
	{
	default:
	case VT_BSTR:
		std::sort(m_indexes.begin(), m_indexes.end(), CBSTRSortComparer(m_pObjectsArray, bstrColumnKey, bDesc));
		break;

	case VT_I8:
	case VT_I4:
	case VT_UI8:
		std::sort(m_indexes.begin(), m_indexes.end(), LONGSortComparer(m_pObjectsArray, bstrColumnKey, bDesc));
		break;
	}
	return S_OK;
}

STDMETHODIMP CVariantTable::ResetSort()
{
	ResetIndexes();
	if(!m_strFilterColumnKey.IsEmpty())
	{
		SetFilter(CComBSTR(m_strFilterColumnKey), &CComVar(m_filterVariant));
	}
	return S_OK;
}

STDMETHODIMP CVariantTable::SetFilter(BSTR bstrColumnKey, VARIANT* value)
{
	CHECK_E_POINTER(value);
	if(value->vt != VT_BSTR)
		return E_NOTIMPL;

	ResetIndexes();
	std::vector<UINT> vIndexes = m_indexes;
	auto itLast = vIndexes.cend();

	auto vt = VT_BSTR;
	if (bstrColumnKey)
	{
		auto it = std::find_if(
			m_columns.begin(),
			m_columns.end(),
			[&](_column_data column_data)
		{
			return column_data.strColumnKey == bstrColumnKey;
		}
		);

		if (it != m_columns.end())
			vt = it->vt;
	}

	switch(vt)
	{
	default:
	case VT_BSTR:
			itLast =  std::partition(
				vIndexes.begin(), 
				vIndexes.end(), 
				BSTRFilterPredicate(m_columns, m_pObjectsArray, bstrColumnKey, value->bstrVal)
			);
		break;

	case VT_I8:
	case VT_I4:
		//filter is not uspported yet
		break;
	}

	m_indexes.clear();
	m_indexes.assign(vIndexes.cbegin(), itLast);
	RestoreSort();
	return S_OK;
}

STDMETHODIMP CVariantTable::ResetFilter()
{
	ResetIndexes();
	RestoreSort();
	return S_OK;
}

STDMETHODIMP CVariantTable::RestoreSort()
{
	if(!m_strSortColumnKey.IsEmpty())
	{
		SetSort(CComBSTR(m_strSortColumnKey), m_bDesc);
	}
	return S_OK;
}
