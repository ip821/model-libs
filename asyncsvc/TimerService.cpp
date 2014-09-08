#include "stdafx.h"
#include "TimerService.h"

STDMETHODIMP CTimerService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	return S_OK;
}

STDMETHODIMP CTimerService::OnShutdown()
{
	StopTimer();
	return S_OK;
}

STDMETHODIMP CTimerService::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult)
{
	if (hWnd == m_hControlWnd && uMsg == WM_TIMER && wParam == (WPARAM)this)
	{
		OnUpdateTimer();
	}
	return S_OK;
}

STDMETHODIMP CTimerService::StartTimer(UINT uiMilliseconds)
{
	m_uiTimeout = uiMilliseconds;
	RETURN_IF_FAILED(OnUpdateTimer());
	return S_OK;
}

STDMETHODIMP CTimerService::ResumeTimer()
{
	SetTimer(m_hControlWnd, (WPARAM)this, m_uiTimeout, NULL);
	return S_OK;
}

STDMETHODIMP CTimerService::StopTimer()
{
	KillTimer(m_hControlWnd, (WPARAM)this);
	return S_OK;
}

STDMETHODIMP CTimerService::OnUpdateTimer()
{
	Fire_OnTimer();
	return S_OK;
}

HRESULT CTimerService::Fire_OnTimer()
{
	HRESULT hr = S_OK;
	CTimerService* pThis = static_cast<CTimerService*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		ITimerServiceEventSink * pConnection = static_cast<ITimerServiceEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnTimer(this);
		}
	}
	return hr;
}