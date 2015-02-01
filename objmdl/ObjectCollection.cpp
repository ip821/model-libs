// ObjectCollection.cpp : Implementation of CObjectCollection

#include "stdafx.h"
#include "ObjectCollection.h"

// CObjectCollection

STDMETHODIMP CObjectCollection::AddObject(IUnknown *punk)
{
	CHECK_E_POINTER(punk);
	m_objects.push_back(CAdapt<CComPtr<IUnknown> >(punk));
	return S_OK;
}

STDMETHODIMP CObjectCollection::InsertObject(IUnknown *punk, UINT uiIndex)
{
	m_objects.insert(m_objects.begin() + uiIndex, CAdapt<CComPtr<IUnknown> >(punk));
	return S_OK;
}

STDMETHODIMP CObjectCollection::Clear()
{
	m_objects.clear();
	return S_OK;
}

STDMETHODIMP CObjectCollection::RemoveObjectAt(UINT uiIndex)
{
	m_objects.erase(m_objects.begin() + uiIndex);
	return S_OK;
}

STDMETHODIMP CObjectCollection::GetCount(UINT *pcObjects)
{
	CHECK_E_POINTER(pcObjects);
	*pcObjects = static_cast<UINT>(m_objects.size());
	return S_OK;
}

STDMETHODIMP CObjectCollection::GetAt(UINT uiIndex, REFIID riid, void **ppv)
{
	CHECK_E_POINTER(ppv);
	return m_objects[uiIndex].m_T->QueryInterface(riid, ppv);
}

STDMETHODIMP CObjectCollection::IndexOf(void* pv, UINT* puiIndex)
{
	CHECK_E_POINTER(pv);
	CHECK_E_POINTER(puiIndex);
	
	auto it = std::find_if(
		m_objects.begin(),
		m_objects.end(),
		[&](CAdapt<CComPtr<IUnknown>>& item)
	{
		return item.m_T.p == pv;
	}
		);

	if (it == m_objects.end())
		return E_NOT_SET;

	*puiIndex = static_cast<UINT>(std::distance(m_objects.begin(), it));
	return S_OK;
}