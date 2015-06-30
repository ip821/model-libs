// CColumnsInfo

#include "stdafx.h"
#include "ColumnsInfo.h"
#include "Metadata.h"

STDMETHODIMP CColumnsInfo::AddItem(IColumnsInfoItem** ppColumnsInfoItem)
{
	CHECK_E_POINTER(ppColumnsInfoItem);
	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ColumnsInfoItem, &pColumnsInfoItem));
	m_pItems.push_back(pColumnsInfoItem);
	RETURN_IF_FAILED(pColumnsInfoItem->QueryInterface(ppColumnsInfoItem));
	return S_OK;
}

STDMETHODIMP CColumnsInfo::FindItemIndex(BSTR bstrName, UINT* puiIndex)
{
	CHECK_E_POINTER(puiIndex);
	for (size_t i = 0; i < m_pItems.size(); i++)
	{
		CComBSTR bstrColumnName;
		RETURN_IF_FAILED(m_pItems[i]->GetRectStringProp(Layout::Metadata::Column::Name, &bstrColumnName));
		if (bstrColumnName == bstrName)
		{
			*puiIndex = i;
			return S_OK;
		}
	}

	return E_NOT_SET;
}

STDMETHODIMP CColumnsInfo::FindItemByName(BSTR bstrName, IColumnsInfoItem** ppColumnsInfoItem)
{
	CHECK_E_POINTER(ppColumnsInfoItem);
	*ppColumnsInfoItem = nullptr;
	for (size_t i = 0; i < m_pItems.size(); i++)
	{
		CComBSTR bstrColumnName;
		RETURN_IF_FAILED(m_pItems[i]->GetRectStringProp(Layout::Metadata::Column::Name, &bstrColumnName));
		if (bstrColumnName == bstrName)
		{
			RETURN_IF_FAILED(m_pItems[i]->QueryInterface(ppColumnsInfoItem));
			return S_OK;
		}

		CComPtr<IColumnsInfo> pChildObjects;
		RETURN_IF_FAILED(m_pItems[i]->GetChildItems(&pChildObjects));
		pChildObjects->FindItemByName(bstrName, ppColumnsInfoItem);
		if (*ppColumnsInfoItem)
			return S_OK;
	}
	return S_OK;
}

STDMETHODIMP CColumnsInfo::GetItem(UINT uiIndex, IColumnsInfoItem** ppColumnsInfoItem)
{
	CHECK_E_POINTER(ppColumnsInfoItem);
	if (uiIndex >= m_pItems.size())
		return E_INVALIDARG;
	RETURN_IF_FAILED(m_pItems[uiIndex]->QueryInterface(ppColumnsInfoItem));
	return S_OK;
}

STDMETHODIMP CColumnsInfo::GetCount(UINT* puiCount)
{
	CHECK_E_POINTER(puiCount);
	*puiCount = m_pItems.size();
	return S_OK;
}

STDMETHODIMP CColumnsInfo::Clear()
{
	m_pItems.clear();
	return S_OK;
}

STDMETHODIMP CColumnsInfo::IsDisabledSelection(BOOL* pbDisabled)
{
	CHECK_E_POINTER(pbDisabled);
	*pbDisabled = m_bDisabledSelection;
	return S_OK;
}

STDMETHODIMP CColumnsInfo::DisableSelection(BOOL bDisabled)
{
	m_bDisabledSelection = bDisabled;
	return S_OK;
}
