// ThreadService.cpp : Implementation of CThreadService

#include "stdafx.h"
#include "ThreadService.h"
#include "Plugins.h"

// CThreadService

STDMETHODIMP CThreadService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	SetTimerService(CLSID_TimerService);
	return S_OK;
}

STDMETHODIMP CThreadService::SetTimerService(GUID gServiceId)
{
	m_pServiceProvider->QueryService(gServiceId, &m_pTimerService);
	if (m_pTimerService)
	{
		CComPtr<IUnknown> pUnk;
		RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
		RETURN_IF_FAILED(AtlAdvise(m_pTimerService, pUnk, __uuidof(ITimerServiceEventSink), &m_dwAdvice));
	}
	return S_OK;
}

STDMETHODIMP CThreadService::SetThreadContext(IVariantObject* pVariantObject)
{
	m_pResult = pVariantObject;
	return S_OK;
}

STDMETHODIMP CThreadService::OnShutdown()
{
	if (m_pTimerService && m_dwAdvice)
		RETURN_IF_FAILED(AtlUnadvise(m_pTimerService, __uuidof(ITimerServiceEventSink), m_dwAdvice));

	m_pTimerService.Release();
	
	if (m_pResult)
		m_pResult.Release();

	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());

	return S_OK;
}

STDMETHODIMP CThreadService::OnTimer(ITimerService* pTimerService)
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	RETURN_IF_FAILED(Fire_OnStart());
	Start();
	return S_OK;
}

STDMETHODIMP CThreadService::Run()
{
	RETURN_IF_FAILED(Fire_OnStart());
	Start();
	return S_OK;
}

HRESULT CThreadService::Fire_OnFinishInternal()
{
	if (m_pResult)
	{
		RETURN_IF_FAILED(Fire_OnFinish());

		CComVariant vRestart;
		m_pResult->GetVariantValue(KEY_RESTART_TIMER, &vRestart);
		if (m_pTimerService && (vRestart.vt == VT_EMPTY || (vRestart.vt == VT_BOOL && vRestart.boolVal)))
		{
			RETURN_IF_FAILED(m_pTimerService->ResumeTimer());
		}
		m_pResult.Release();
	}
	return S_OK;
}

STDMETHODIMP CThreadService::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult)
{
	if (hWnd == m_hControlWnd && uMsg == WM_FINISHED && wParam == (WPARAM)this)
	{
		RETURN_IF_FAILED(Fire_OnFinishInternal());
	}
	return S_OK;
}

void CThreadService::OnRun()
{
	m_hr = S_OK;
	m_hr = Fire_OnRun();

	CComPtr<IErrorInfo> pErrorInfo;
	GetErrorInfo(0, &pErrorInfo);
	auto errInfo = _com_error(m_hr, pErrorInfo, true);
	CComBSTR bstrDesc(errInfo.Description().Detach());

	m_pResult->SetVariantValue(KEY_HRESULT, &CComVariant(m_hr));

	if (bstrDesc == CComBSTR(L""))
	{
		bstrDesc = errInfo.ErrorMessage();
	}

	m_pResult->SetVariantValue(KEY_HRESULT_DESCRIPTION, &CComVariant(bstrDesc));
}

void CThreadService::OnStop()
{
	if (m_hControlWnd)
		::PostMessage(m_hControlWnd, WM_FINISHED, (WPARAM)this, 0);
	else
		Fire_OnFinishInternal();
}

HRESULT CThreadService::Fire_OnStart()
{
	if (!m_pResult)
	{
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &m_pResult));
	}

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pResult->SetVariantValue(KEY_THREAD_ID, &CComVariant(pUnk)));
	HRESULT hr = S_OK;
	CThreadService* pThis = static_cast<CThreadService*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		IThreadServiceEventSink * pConnection = static_cast<IThreadServiceEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnStart(m_pResult);
		}
	}
	return hr;
}

HRESULT CThreadService::Fire_OnFinish()
{
	HRESULT hr = S_OK;
	CThreadService* pThis = static_cast<CThreadService*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		IThreadServiceEventSink * pConnection = static_cast<IThreadServiceEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnFinish(m_pResult);
		}
	}
	return hr;
}

HRESULT CThreadService::Fire_OnRun()
{
	HRESULT hr = S_OK;
	CThreadService* pThis = static_cast<CThreadService*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		IThreadServiceEventSink * pConnection = static_cast<IThreadServiceEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnRun(m_pResult);
			if (FAILED(hr))
			{
				break;
			}
		}
	}
	return hr;
}

STDMETHODIMP CThreadService::Join()
{
	if (m_thread.joinable())
		m_thread.join();
	return S_OK;
}
