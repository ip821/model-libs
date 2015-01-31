#pragma once

#include <functional>
#include <ShlObj.h>

#include "objmdl_contract_i.h"
#include "viewmdl_contract_i.h"

static HRESULT HrAddColumn(IPluginManager* pPluginManager, IVariantObject* pVariantObject, CComBSTR bstrKey, CString strName, VARTYPE vt = VT_BSTR, int iWidth = LVSCW_AUTOSIZE)
{
	CHECK_E_POINTER(pPluginManager);
	CHECK_E_POINTER(pVariantObject);
	CComVariant v;
	CComPtr<IObjCollection> pObjectCollectionColumns;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(IP::ObjectModel::Metadata::TableObject::ColumnsObject, &v));
	if(v.vt == VT_EMPTY)
	{
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollectionColumns));
		v = pObjectCollectionColumns;
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(IP::ObjectModel::Metadata::TableObject::ColumnsObject, &v));
	}
	else
	{
		RETURN_IF_FAILED(v.punkVal->QueryInterface(IID_IObjCollection, (LPVOID*)&pObjectCollectionColumns));
	}

	CComPtr<IVariantObject> pVariantObjectColumn;
	RETURN_IF_FAILED(pPluginManager->CoCreateInstance(CLSID_VariantObject, IID_IVariantObject, (LPVOID*)&pVariantObjectColumn));
	pObjectCollectionColumns->AddObject(pVariantObjectColumn);
	RETURN_IF_FAILED(pVariantObjectColumn->SetVariantValue(IP::ObjectModel::Metadata::Table::Column::Name, &CComVariant(strName)));
	RETURN_IF_FAILED(pVariantObjectColumn->SetVariantValue(IP::ObjectModel::Metadata::Table::Column::Key, &CComVariant(bstrKey)));
	RETURN_IF_FAILED(pVariantObjectColumn->SetVariantValue(IP::ObjectModel::Metadata::Table::Column::Type, &CComVariant(vt)));
	RETURN_IF_FAILED(pVariantObjectColumn->SetVariantValue(IP::ObjectModel::Metadata::Table::Column::Width, &CComVariant(iWidth)));
	return S_OK;
}

static HRESULT HrInitializeWithSettings(IUnknown* pObject, ISettings* pSettings)
{
	CHECK_E_POINTER(pObject);
	CHECK_E_POINTER(pSettings);
	CComQIPtr<IInitializeWithSettings> pObj = pObject;
	if (pObj)
	{
		RETURN_IF_FAILED(pObj->Load(pSettings));
	}
	return S_OK;
}

static HRESULT HrInitializeWithControl(IUnknown* pObject, IUnknown* pControl)
{
	CHECK_E_POINTER(pObject);
	CHECK_E_POINTER(pControl);
	CComQIPtr<IInitializeWithControl> pObj = pObject;
	CComQIPtr<IControl> pCtrl = pControl;
	if (pObj)
	{
		RETURN_IF_FAILED(pObj->SetControl(pCtrl));
	}
	return S_OK;
}

static HRESULT HrNotifyOnInitialized(IUnknown* pObject, IUnknown* pServiceProviderObj)
{
	CHECK_E_POINTER(pObject);
	CHECK_E_POINTER(pServiceProviderObj);
	CComQIPtr<IPluginSupportNotifications> pObj = pObject;
	CComQIPtr<IServiceProvider> pServiceProvider = pServiceProviderObj;
	if (pObj)
	{
		RETURN_IF_FAILED(pObj->OnInitialized(pServiceProvider));
	}
	return S_OK;
}

static HRESULT HrWrapToVariantTable(IPluginManager* pPluginManager, IVariantObject* pVariantObject, IObjCollection* pObjectCollection, IVariantTable** ppVariantTable)
{
	if (!pObjectCollection)
		return E_POINTER;
	if (!ppVariantTable)
		return E_POINTER;

	CComVariant v;
	CComPtr<IObjCollection> pObjectCollectionColumns;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(IP::ObjectModel::Metadata::TableObject::ColumnsObject, &v));
	RETURN_IF_FAILED(v.punkVal->QueryInterface(IID_IObjCollection, (LPVOID*)&pObjectCollectionColumns));

	CComQIPtr<IObjArray> pObjectArray = pObjectCollection;
	if (!pObjectArray)
		return E_NOINTERFACE;

	CComPtr<IVariantTable> pVariantTable;
	RETURN_IF_FAILED(pPluginManager->CoCreateInstance(CLSID_VariantTable, IID_IVariantTable, (LPVOID*)&pVariantTable));

	RETURN_IF_FAILED(pVariantTable->SetColumns(pObjectCollectionColumns));
	RETURN_IF_FAILED(pVariantTable->SetObjects(pObjectArray));

	RETURN_IF_FAILED(pVariantTable->QueryInterface(IID_IVariantTable, (LPVOID*)ppVariantTable));
	return S_OK;
}

template<class T>
HRESULT HrIteratePlugins(IPluginSupport* pPluginSupport, std::function<HRESULT(T*)> func)
{
	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(pPluginSupport->GetPlugins(&pObjectArray));
	UINT cb = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&cb));
	for (UINT i = 0; i < cb; i++)
	{
		CComPtr<T> p;
		HRESULT hr = pObjectArray->GetAt(i, __uuidof(T), (LPVOID*)&p);
		if (hr == E_NOINTERFACE)
			continue;

		if (p)
		{
			HRESULT hr = func(p);
			if (hr == E_NOTIMPL)
				continue;
			RETURN_IF_FAILED(hr);
		}
	}
	return S_OK;
}