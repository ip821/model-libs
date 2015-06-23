// VariantObject.cpp : Implementation of CVariantObject

#include "stdafx.h"
#include "VariantObject.h"


// CVariantObject

STDMETHODIMP CVariantObject::Clear()
{
	m_values.clear();
	return S_OK;
}

STDMETHODIMP CVariantObject::CopyTo(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);

	for(auto it = m_values.begin(); it != m_values.end(); it++)
	{
		CComBSTR key(it->first);
		CComVariant v;
		RETURN_IF_FAILED(GetVariantValue(key, &v));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(key, &v));
	}
	return S_OK;
}

STDMETHODIMP CVariantObject::GetVariantValue(BSTR key, VARIANT* v)
{
	CHECK_E_POINTER(key);
	CHECK_E_POINTER(v);
	auto it = m_values.find(key);
	if(it == m_values.end())
	{
		return ::VariantClear(v);
	}
	
	CComVariant cv(it->second);
	RETURN_IF_FAILED(cv.Detach(v));
	return S_OK;
}

STDMETHODIMP CVariantObject::SetVariantValue(BSTR key, VARIANT* v)
{
	CHECK_E_POINTER(key);
	CHECK_E_POINTER(v);
	if (v->vt == VT_BOOL && v->boolVal == ATL_VARIANT_TRUE)
		v->boolVal = TRUE;
	m_values[key] = *v;
	return S_OK;
}

STDMETHODIMP CVariantObject::RemoveVariantValue(BSTR key)
{
	CHECK_E_POINTER(key);
	m_values.erase(key);
	return S_OK;
}

STDMETHODIMP CVariantObject::GetCount(UINT_PTR* puiCount)
{
	CHECK_E_POINTER(puiCount);
	*puiCount = m_values.size();
	return S_OK;
}

STDMETHODIMP CVariantObject::GetKeyByIndex(UINT_PTR uiIndex, BSTR* pbstrKey)
{
	CHECK_E_POINTER(pbstrKey);
	auto it = m_values.begin();
	std::advance(it, uiIndex);
	if (it == m_values.end())
		return E_INVALIDARG;
	*pbstrKey = CComBSTR(it->first).Copy();
	return S_OK;
}
