#pragma once

struct _column_data
{
	CComBSTR strColumnKey;
	VARENUM vt;
	CComBSTR strColumnName;
};


class BSTRFilterPredicate
{
private:
	CComPtr<IObjArray> m_pObjectArray;
	CComBSTR m_bstrColumnKey;
	BOOL m_bDesc;
	CComBSTR m_strValue;
	std::vector<_column_data> m_columns;

public:
	BSTRFilterPredicate(std::vector<_column_data>& columns, IObjArray* pObjectArray, BSTR bstrColumnKey, BSTR strValue) :
		m_columns(columns),
		m_pObjectArray(pObjectArray),
		m_bstrColumnKey(bstrColumnKey),
		m_strValue(strValue)
	{
	}

	bool operator()(const UINT& index) const
	{
		if(m_bstrColumnKey != NULL)
			return OneColumnFind(index, m_bstrColumnKey);

		return AllColumnsFind(index);
	}

	bool AllColumnsFind(const UINT& index) const
	{
		for(auto it = m_columns.begin(); it != m_columns.end(); it++)
		{
			if(it->vt != VT_BSTR)
				continue;

			if(OneColumnFind(index, it->strColumnKey))
				return true;
		}
		return false;
	}

	bool OneColumnFind(const UINT& index, BSTR bstrColumnKey) const
	{
		CComPtr<IVariantObject> pVariantObject;
		m_pObjectArray->GetAt(index, IID_IVariantObject, (LPVOID*)&pVariantObject);
		CComVar v;
		pVariantObject->GetVariantValue(bstrColumnKey, &v);

		return CString(v.bstrVal).MakeLower().Find(CString(m_strValue).MakeLower()) != -1;
	}
};