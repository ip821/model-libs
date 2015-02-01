// FormManager.cpp : Implementation of CFormManager

#include "stdafx.h"
#include "FormManager.h"


// CFormManager

STDMETHODIMP CFormManager::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(GetContainerControl(&pContainerControl));

	m_pTabbedControl = pContainerControl;
	CComPtr<IUnknown> pThis;
	RETURN_IF_FAILED(QueryInterface(IID_PPV_ARGS(&pThis)));
	RETURN_IF_FAILED(AtlAdvise(m_pTabbedControl, pThis, IID_ITabbedControlEventSink, &m_dwAdvice));
	return S_OK;
}

STDMETHODIMP CFormManager::OnShutdown()
{
	AtlUnadvise(m_pTabbedControl, IID_ITabbedControlEventSink, m_dwAdvice);
	m_pTabbedControl.Release();
	return S_OK;
}

STDMETHODIMP CFormManager::GetContainerControl(IContainerControl** ppContainerControl)
{
	CComQIPtr<IContainerControl> pContainerControl = m_pControl;
	if (m_pControl->QueryInterface(ppContainerControl) != S_OK)
	{
		CComQIPtr<IMainWindow> pMainWindow = m_pControl;
		if (pMainWindow == NULL)
			return E_NOINTERFACE;

		RETURN_IF_FAILED(pMainWindow->GetContainerControl(ppContainerControl));
	}
	return S_OK;
}

STDMETHODIMP CFormManager::CloseAll()
{
	DWORD dwCount = 0;
	RETURN_IF_FAILED(m_pTabbedControl->GetPageCount(&dwCount));
	while (dwCount != 0)
	{
		CComPtr<IControl> pControl;
		RETURN_IF_FAILED(m_pTabbedControl->GetPage(0, &pControl));
		RETURN_IF_FAILED(m_pTabbedControl->RemovePage(pControl));
		dwCount--;
	}
	return S_OK;
}

STDMETHODIMP CFormManager::CloseForm(IControl* pControl)
{
	CHECK_E_POINTER(pControl);
	RETURN_IF_FAILED(m_pTabbedControl->RemovePage(pControl));
	return S_OK;
}

STDMETHODIMP CFormManager::OnTabHeaderClick(IControl* pControl)
{
	UNREFERENCED_PARAMETER(pControl);
	return S_OK;
}

STDMETHODIMP CFormManager::OnActivate(IControl* pControl)
{
	UNREFERENCED_PARAMETER(pControl);
	RETURN_IF_FAILED(Fire_OnActivate(pControl));
	return S_OK;
}

STDMETHODIMP CFormManager::OnDeactivate(IControl* pControl)
{
	UNREFERENCED_PARAMETER(pControl);
	RETURN_IF_FAILED(Fire_OnDeactivate(pControl));
	return S_OK;
}

STDMETHODIMP CFormManager::OnClose(IControl* pControl)
{
	CHECK_E_POINTER(pControl);
	for(auto it = m_pControls.begin(); it != m_pControls.end(); it++)
	{
		if(it->second.m_T == pControl)
		{
			m_pControls.erase(it);
			break;
		}
	}

	return S_OK;
}


STDMETHODIMP CFormManager::ActivateForm2(IControl* pControl)
{
	CHECK_E_POINTER(pControl);
	RETURN_IF_FAILED(m_pTabbedControl->ActivatePage(pControl));
	return S_OK;
}

STDMETHODIMP CFormManager::ActivateForm(GUID guidId)
{
	auto it = m_pControls.find(guidId);
	if (it == m_pControls.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	RETURN_IF_FAILED(m_pTabbedControl->ActivatePage(it->second.m_T));
	return S_OK;
}

STDMETHODIMP CFormManager::OpenForm2(GUID guidNamespace, GUID guidType, GUID guidId, IControl** ppControl)
{
	CHECK_E_POINTER(ppControl);
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(GetPluginManager()->CreatePluginInstance(guidNamespace, guidType, guidId, &pUnk));
	CComQIPtr<IControl> pControl = pUnk;
	m_pControls[guidId] = pControl;
	RETURN_IF_FAILED(OpenFormInternal(pControl));
	return pControl->QueryInterface(IID_IControl, (LPVOID*)ppControl);
}

STDMETHODIMP CFormManager::OpenFormInternal(IControl* pControl)
{
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(GetContainerControl(&pContainerControl));

	CComQIPtr<ITabbedControl> pTabbedControl = pContainerControl;
	ATLENSURE(pTabbedControl);

	CComQIPtr<IControl> pParentControl = pContainerControl;
	CComQIPtr<IInitializeWithControl> pInitializeWithControl = pControl;
	if (pParentControl && pInitializeWithControl)
	{
		RETURN_IF_FAILED(pInitializeWithControl->SetControl(pParentControl));
	}

	RETURN_IF_FAILED(pTabbedControl->AddPage(pControl));
	return S_OK;
}

STDMETHODIMP CFormManager::OpenForm(GUID guidId, IControl** ppControl)
{
	CHECK_E_POINTER(ppControl);
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(HrCoCreateInstance(guidId, &pControl));
	m_pControls[guidId] = pControl;
	RETURN_IF_FAILED(OpenFormInternal(pControl));
	return pControl->QueryInterface(IID_IControl, (LPVOID*)ppControl);
}

STDMETHODIMP CFormManager::FindForm(GUID guidId, IControl** ppControl)
{
	CHECK_E_POINTER(ppControl);
	auto it = m_pControls.find(guidId);
	if(it == m_pControls.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	return it->second.m_T->QueryInterface(IID_IControl, (LPVOID*)ppControl);
}

HRESULT CFormManager::Fire_OnActivate(IControl* pControl)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CFormManager* pThis = static_cast<CFormManager*>(this);
	int cConnections = IConnectionPointImpl_IFormManagerEventSink::m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = IConnectionPointImpl_IFormManagerEventSink::m_vec.GetAt(iConnection);
		pThis->Unlock();

		IFormManagerEventSink* pConnection = static_cast<IFormManagerEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnActivate(pControl);
		}
	}
	return hr;
}

HRESULT CFormManager::Fire_OnDeactivate(IControl* pControl)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CFormManager* pThis = static_cast<CFormManager*>(this);
	int cConnections = IConnectionPointImpl_IFormManagerEventSink::m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = IConnectionPointImpl_IFormManagerEventSink::m_vec.GetAt(iConnection);
		pThis->Unlock();

		IFormManagerEventSink* pConnection = static_cast<IFormManagerEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnDeactivate(pControl);
		}
	}
	return hr;
}