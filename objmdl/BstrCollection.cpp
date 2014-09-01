// BstrCollection.cpp : Implementation of CBstrCollection

#include "stdafx.h"
#include "BstrCollection.h"


// CBstrCollection

STDMETHODIMP CBstrCollection::AddItem(BSTR bstr)
{
	CHECK_E_POINTER(bstr);
	m_vector.push_back(CComBSTR(bstr));
	return S_OK;
}

STDMETHODIMP CBstrCollection::GetCount(UINT_PTR* pdwCount)
{
	CHECK_E_POINTER(pdwCount);
	*pdwCount = m_vector.size();
	return S_OK;
}

STDMETHODIMP CBstrCollection::GetItem(UINT_PTR dwIndex, BSTR* pbstr)
{
	CHECK_E_POINTER(pbstr);
	if(dwIndex >= m_vector.size())
		return E_INVALIDARG;
	*pbstr = m_vector[dwIndex].m_T.Copy();
	return S_OK;
}

STDMETHODIMP CBstrCollection::RemoveAt(UINT_PTR dwIndex)
{
	if(dwIndex < m_vector.size())
		return E_INVALIDARG;
	m_vector.erase(m_vector.begin() + dwIndex);
	return S_OK;
}

