// PluginSupport.cpp : Implementation of CPluginSupport

#include "stdafx.h"
#include "PluginSupport.h"
#include "Functions.h"

// CPluginSupport

HRESULT CPluginSupport::FinalConstruct()
{
	RETURN_IF_FAILED(HrGetPluginManager(&m_pPluginManager));
	return S_OK;
}

STDMETHODIMP CPluginSupport::GetPlugins(IObjArray** ppObjectArray)
{
	CHECK_E_POINTER(ppObjectArray);
	CComPtr<IObjCollection> pObjectCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollection));

	for(auto it = m_Plugins.begin(); it != m_Plugins.end(); it++)
	{
		RETURN_IF_FAILED(pObjectCollection->AddObject(it->m_T));
	}

	return pObjectCollection->QueryInterface(IID_IObjArray, (LPVOID*)ppObjectArray);
}

STDMETHODIMP CPluginSupport::OnShutdown()
{
	for (auto it = m_Plugins.cbegin(); it != m_Plugins.cend(); it++)
	{
		CComQIPtr<IPluginSupportNotifications> pPluginSupportNotifications = it->m_T;
		if (pPluginSupportNotifications)
		{
			RETURN_IF_FAILED(pPluginSupportNotifications->OnShutdown());
		}
	}

	m_Plugins.clear();
	m_PluginsMap.clear();
	m_pControl.Release();
	m_pSettings.Release();
	m_pPluginManager.Release();
	m_pServiceProvider.Release();

	return S_OK;
}

STDMETHODIMP CPluginSupport::OnInitCompleted()
{
	for (auto it = m_Plugins.cbegin(); it != m_Plugins.cend(); it++)
	{
		CComQIPtr<IPluginSupportNotifications2> pPluginSupportNotifications = it->m_T;
		if (pPluginSupportNotifications)
		{
			RETURN_IF_FAILED(pPluginSupportNotifications->OnInitCompleted());
		}
	}

	return S_OK;
}

STDMETHODIMP CPluginSupport::OnInitializing()
{
	for (auto it = m_Plugins.cbegin(); it != m_Plugins.cend(); it++)
	{
		CComQIPtr<IPluginSupportNotifications2> pPluginSupportNotifications = it->m_T;
		if (pPluginSupportNotifications)
		{
			RETURN_IF_FAILED(pPluginSupportNotifications->OnInitializing(this));
		}
	}

	return S_OK;
}

STDMETHODIMP CPluginSupport::OnInitialized()
{
	for(auto it = m_Plugins.cbegin(); it != m_Plugins.cend(); it++)
	{
		CComQIPtr<IPluginSupportNotifications> pPluginSupportNotifications = it->m_T;
		if(pPluginSupportNotifications)
		{
			RETURN_IF_FAILED(pPluginSupportNotifications->OnInitialized(this));
		}
	}

	return S_OK;
}

STDMETHODIMP CPluginSupport::InitializePlugins(REFGUID guidNamespace, REFGUID guidType)
{
	CComPtr<IObjArray> pObjectArray;
	HRESULT hr = m_pPluginManager->GetPluginInfoCollection(guidNamespace, guidType, &pObjectArray);
	if(hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		return S_OK;

	RETURN_IF_FAILED(hr);

	UINT cObjects = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&cObjects));
	for(UINT i = 0; i < cObjects; i++)
	{
		CComPtr<IPluginInfo> pPluginInfo;
		hr = pObjectArray->GetAt(i, IID_IPluginInfo, (LPVOID*)&pPluginInfo);
		if(FAILED(hr))
			continue;

		GUID guidId = {0};
		hr = pPluginInfo->GetId(&guidId);
		if(FAILED(hr))
			continue;

		CComPtr<IUnknown> pUnk;
		hr = pPluginInfo->CreateInstance(&pUnk);
		if(FAILED(hr))
			continue;

		if (m_pControl)
		{
			RETURN_IF_FAILED(HrInitializeWithControl(pUnk, m_pControl));
		}

		if (m_pSettings)
		{
			RETURN_IF_FAILED(HrInitializeWithSettings(pUnk, m_pSettings));
		}

		if (m_pVariantObject)
		{
			RETURN_IF_FAILED(HrInitializeWithVariantObject(pUnk, m_pVariantObject));
		}

		m_PluginsMap[guidId] = pUnk;
		m_Plugins.push_back(pUnk);
	}
	return S_OK;
}

STDMETHODIMP CPluginSupport::Load(ISettings* pSettings)
{
	m_pSettings = pSettings;
	for (auto it = m_Plugins.begin(); it != m_Plugins.end(); it++)
	{
		RETURN_IF_FAILED(HrInitializeWithSettings(it->m_T, m_pSettings));
	}
	return S_OK;
}

STDMETHODIMP CPluginSupport::SetControl(IControl* pControl)
{
	m_pControl = pControl;
	for(auto it = m_Plugins.begin(); it != m_Plugins.end(); it++)
	{
		RETURN_IF_FAILED(HrInitializeWithControl(it->m_T, m_pControl));
	}
	return S_OK;
}

STDMETHODIMP CPluginSupport::SetVariantObject(IVariantObject* pVariantObject)
{
	m_pVariantObject = pVariantObject;
	for (auto it = m_Plugins.begin(); it != m_Plugins.end(); it++)
	{
		RETURN_IF_FAILED(HrInitializeWithVariantObject(it->m_T, m_pVariantObject));
	}
	return S_OK;
}

STDMETHODIMP CPluginSupport::SetParentServiceProvider(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	return S_OK;
}

STDMETHODIMP CPluginSupport::QueryService(REFGUID guidService, REFIID iid, void** ppServiceObject)
{
	CHECK_E_POINTER(ppServiceObject);
	auto it = m_PluginsMap.find(guidService);
	if (it != m_PluginsMap.end())
	{ //found
		return it->second.m_T->QueryInterface(iid, ppServiceObject);
	}

	//search in parent
	if (!m_pServiceProvider || FAILED(m_pServiceProvider->QueryService(guidService, iid, ppServiceObject)))
	{
		CComQIPtr<IServiceProvider> pServiceProvider = m_pControl;
		if (pServiceProvider == this)
			return E_INVALIDARG;

		if (pServiceProvider)
		{
			RETURN_IF_FAILED(pServiceProvider->QueryService(guidService, iid, ppServiceObject));
			return S_OK;
		}
	}
	return S_OK;
}
