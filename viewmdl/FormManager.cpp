// FormManager.cpp : Implementation of CFormManager

#include "stdafx.h"
#include "FormManager.h"


// CFormManager

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
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(GetContainerControl(&pContainerControl));

	CComQIPtr<ITabbedControl> pTabbedControl = pContainerControl;
	ATLENSURE(pTabbedControl);
	DWORD dwCount = 0;
	RETURN_IF_FAILED(pTabbedControl->GetPageCount(&dwCount));
	while (dwCount != 0)
	{
		CComPtr<IControl> pControl;
		RETURN_IF_FAILED(pTabbedControl->GetPage(0, &pControl));
		RETURN_IF_FAILED(pTabbedControl->RemovePage(pControl));
		dwCount--;
	}
	return S_OK;
}

STDMETHODIMP CFormManager::OnDeactivate(IControl* pControl)
{
	UNREFERENCED_PARAMETER(pControl);
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

	if(!m_pControls.size() && m_dwAdvice)
	{
		CComPtr<IPluginManager> pPluginManager;
		RETURN_IF_FAILED(HrGetPluginManager(&pPluginManager));

		CComPtr<IContainerControl> pContainerControl;
		RETURN_IF_FAILED(GetContainerControl(&pContainerControl));

		AtlUnadvise(pContainerControl, IID_ITabbedControlEventSink, m_dwAdvice);
		m_dwAdvice = 0;
	}
	return S_OK;
}

STDMETHODIMP CFormManager::ActivateForm(GUID guidId)
{
	auto it = m_pControls.find(guidId);
	if (it == m_pControls.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(GetContainerControl(&pContainerControl));
	CComQIPtr<ITabbedControl> pTabbedControl = pContainerControl;
	if (!pTabbedControl)
		return E_NOINTERFACE;

	RETURN_IF_FAILED(pTabbedControl->ActivatePage(it->second.m_T));
	return S_OK;
}

STDMETHODIMP CFormManager::OpenForm(GUID guidId, IControl** ppControl)
{
	CHECK_E_POINTER(ppControl);
	CComPtr<IPluginManager> pPluginManager;
	RETURN_IF_FAILED(HrGetPluginManager(&pPluginManager));

	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(GetContainerControl(&pContainerControl));

	CComQIPtr<ITabbedControl> pTabbedControl = pContainerControl;
	ATLENSURE(pTabbedControl);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(pPluginManager->CreatePluginInstance(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, guidId, &pUnk));

	CComQIPtr<IControl> pControl = pUnk;
	ATLENSURE(pControl);

	CComQIPtr<IControl> pParentControl = pContainerControl;
	CComQIPtr<IInitializeWithControl> pInitializeWithControl = pControl;
	if(pParentControl && pInitializeWithControl)
	{
		RETURN_IF_FAILED(pInitializeWithControl->SetControl(pParentControl));
	}

	RETURN_IF_FAILED(pTabbedControl->AddPage(pControl));

	m_pControls[guidId] = pControl;
	if(!m_dwAdvice)
	{
		CComPtr<IUnknown> pThis;
		RETURN_IF_FAILED(QueryInterface(IID_PPV_ARGS(&pThis)));
		RETURN_IF_FAILED(pContainerControl.Advise(pThis, IID_ITabbedControlEventSink, &m_dwAdvice));
	}

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