// VariantCollection.cpp : Implementation of CVariantCollection

#include "stdafx.h"
#include "VariantCollection.h"


// CVariantCollection

STDMETHODIMP CVariantCollection::AddItem(VARIANT* pv)
{
	CHECK_E_POINTER(pv);
	m_vector.push_back(CComVar(*pv));
	return S_OK;
}

STDMETHODIMP CVariantCollection::GetCount(ULONG* pulCount)
{
	CHECK_E_POINTER(pulCount);
	*pulCount = static_cast<ULONG>(m_vector.size());
	return S_OK;
}

STDMETHODIMP CVariantCollection::GetItem(ULONG ulIndex, VARIANT* pv)
{
	CHECK_E_POINTER(pv);
	*pv = m_vector[ulIndex];
	return S_OK;
}

STDMETHODIMP CVariantCollection::RemoveAt(ULONG ulIndex)
{
	if (ulIndex < m_vector.size())
		return E_INVALIDARG;
	m_vector.erase(m_vector.begin() + ulIndex);
	return S_OK;
}

