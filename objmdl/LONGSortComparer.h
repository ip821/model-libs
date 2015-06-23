#pragma once

class LONGSortComparer
{
private:
	CComPtr<IObjArray> m_pObjectArray;
	CComBSTR m_bstrColumnKey;
	BOOL m_bDesc;
	LCID m_lcid;
public:
	LONGSortComparer(IObjArray* pObjectArray, BSTR bstrColumnKey, BOOL bDesc) :
		m_pObjectArray(pObjectArray),
		m_bstrColumnKey(bstrColumnKey),
		m_bDesc(bDesc),
		m_lcid(::GetThreadLocale())
	{
	}

	bool operator()(const UINT& index1, const UINT& index2)const
	{
		CComPtr<IVariantObject> pVariantObject1;
		m_pObjectArray->GetAt(index1, IID_IVariantObject, (LPVOID*)&pVariantObject1);
		CComVar v1;
		pVariantObject1->GetVariantValue(m_bstrColumnKey, &v1);

		CComPtr<IVariantObject> pVariantObject2;
		m_pObjectArray->GetAt(index2, IID_IVariantObject, (LPVOID*)&pVariantObject2);
		CComVar v2;
		pVariantObject2->GetVariantValue(m_bstrColumnKey, &v2);

		auto bRes = false;
		switch (v1.vt)
		{
		case VT_I4:
			bRes = v1.intVal < v2.intVal;
			break;

		case VT_I8:
			bRes = v1.llVal < v2.llVal;
			break;

		case VT_UI8:
			bRes = v1.ullVal < v2.ullVal;
			break;

		default:
			break;
		}
		

		return !m_bDesc ? bRes : !bRes;
	}
};