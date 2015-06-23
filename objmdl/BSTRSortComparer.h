#pragma once

#include "ObjMdl_i.h"

class CBSTRSortComparer
{
private:
	CComPtr<IObjArray> m_pObjectArray;
	CComBSTR m_bstrColumnKey;
	BOOL m_bDesc;
	LCID m_lcid;
public:
	CBSTRSortComparer(IObjArray* pObjectArray, BSTR bstrColumnKey, BOOL bDesc) :
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

		auto bRes = VarBstrCmp(v1.bstrVal, v2.bstrVal, m_lcid, NORM_IGNORECASE) == VARCMP_LT;

		return !m_bDesc ? bRes : !bRes;
	}
};