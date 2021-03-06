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
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pTimerService, __uuidof(ITimerServiceEventSink), m_dwAdvice));
	}

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_hControlWnd = NULL;
		StopInternal();

		if (m_pResult)
			m_pResult.Release();

		RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
		m_pTimerService.Release();
		m_pServiceProvider.Release();
	}
	return S_OK;
}

void CThreadService::JoinAndStop(bool bJoin)
{
	auto handle = m_handle;
	if (handle)
	{
		if (bJoin)
			WaitForSingleObject(handle, INFINITE);

		m_handle = 0;
		CloseHandle(handle);
	}
}

void CThreadService::StopInternal()
{
	JoinAndStop(false);
}

unsigned CThreadService::ThreadProc(void* pThis)
{
	CThreadService* pThreadOperation = static_cast<CThreadService*>(pThis);
	pThreadOperation->RunInternal();
	return 0;
}

void CThreadService::StartInternal()
{
	JoinAndStop();
	m_handle = (HANDLE)_beginthreadex(nullptr, 0, &CThreadService::ThreadProc, this, 0, nullptr);
}

void CThreadService::RunInternal()
{
	CComQIPtr<IThreadService> pGuard = this;
	g_guard = pGuard;

	OnRun();
	OnStop();

	auto handle = m_handle;
	m_handle = 0;
	if (handle)
		CloseHandle(handle);
}

STDMETHODIMP CThreadService::OnTimer(ITimerService* /*pTimerService*/)
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	RETURN_IF_FAILED(Fire_OnStart());
	StartInternal();
	return S_OK;
}

STDMETHODIMP CThreadService::Run()
{
	HWND hWnd = 0;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		hWnd = m_hControlWnd;
	}
	if (hWnd)
		::SendMessage(hWnd, WM_STARTING, (WPARAM)this, 0);
	else
		RETURN_IF_FAILED(Fire_OnStart());
	StartInternal();
	return S_OK;
}

HRESULT CThreadService::Fire_OnFinishInternal()
{
	CComPtr<IVariantObject> pResult = m_pResult;
	if (pResult)
	{
		RETURN_IF_FAILED(Fire_OnFinish());

		CComVar vRestart;
		RETURN_IF_FAILED(pResult->GetVariantValue(Metadata::Timer::RestartFlag, &vRestart));
		if (m_pTimerService && (vRestart.vt == VT_EMPTY || (vRestart.vt == VT_BOOL && vRestart.boolVal)))
		{
			RETURN_IF_FAILED(m_pTimerService->ResumeTimer());
		}
		m_pResult.Release();
	}
	return S_OK;
}

STDMETHODIMP CThreadService::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* /*plResult*/, BOOL* /*bResult*/)
{
	if (hWnd == m_hControlWnd && uMsg == WM_FINISHED && wParam == (WPARAM)this)
	{
		RETURN_IF_FAILED(Fire_OnFinishInternal());
	}
	if (hWnd == m_hControlWnd && uMsg == WM_STARTING && wParam == (WPARAM)this)
	{
		RETURN_IF_FAILED(Fire_OnStart());
	}
	return S_OK;
}

void CThreadService::OnRun()
{
	CCoInitializeScope comScope;
	CComPtr<IVariantObject> pResult = m_pResult;

	m_hr = S_OK;
	m_hr = Fire_OnRun();

	CComPtr<IErrorInfo> pErrorInfo;
	ASSERT_IF_FAILED(GetErrorInfo(0, &pErrorInfo));
	auto errInfo = _com_error(m_hr, pErrorInfo, true);
	CComBSTR bstrDesc(errInfo.Description().Detach());

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);

		if (!pResult)
			return;

		pResult->SetVariantValue(Metadata::Thread::HResult, &CComVar(m_hr));

		if (bstrDesc == CComBSTR(L""))
		{
			bstrDesc = errInfo.ErrorMessage();
		}

		pResult->SetVariantValue(Metadata::Thread::HResultDescription, &CComVar(bstrDesc));
	}
}

void CThreadService::OnStop()
{
	HWND hWnd = 0;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		hWnd = m_hControlWnd;
	}

	if (hWnd)
		::SendMessage(hWnd, WM_FINISHED, (WPARAM)this, 0);
	else
		Fire_OnFinishInternal();
	g_guard.Release();
}

HRESULT CThreadService::Fire_OnStart()
{
	if (!m_pResult)
	{
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &m_pResult));
	}

	CComPtr<IVariantObject> pResult = m_pResult;
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pResult->SetVariantValue(Metadata::Thread::Id, &CComVar(pUnk)));
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
			hr = pConnection->OnStart(pResult);
		}
	}
	return hr;
}

HRESULT CThreadService::Fire_OnFinish()
{
	HRESULT hr = S_OK;
	CThreadService* pThis = static_cast<CThreadService*>(this);
	int cConnections = m_vec.GetSize();

	CComPtr<IVariantObject> pResult = m_pResult;
	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		IThreadServiceEventSink * pConnection = static_cast<IThreadServiceEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnFinish(pResult);
		}
	}
	return hr;
}

HRESULT CThreadService::Fire_OnRun()
{
	HRESULT hr = S_OK;
	CThreadService* pThis = static_cast<CThreadService*>(this);
	vector<CComPtr<IUnknown>> vec;
	pThis->Lock();
	int cConnections = m_vec.GetSize();
	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		vec.push_back(punkConnection);
	}
	pThis->Unlock();

	CComPtr<IVariantObject> pResult = m_pResult;
	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		CComPtr<IUnknown> punkConnection = vec.at(iConnection);

		IThreadServiceEventSink * pConnection = static_cast<IThreadServiceEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnRun(pResult);
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
	if (m_handle)
	{
		WaitForSingleObject(m_handle, INFINITE);
	}
	return S_OK;
}
