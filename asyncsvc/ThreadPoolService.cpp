// ThreadPoolService.cpp : Implementation of CThreadPoolService

#include "stdafx.h"
#include "ThreadPoolService.h"
#include "Plugins.h"

// CThreadPoolService

STDMETHODIMP CThreadPoolService::OnInitialized(IServiceProvider* /*pServiceProvider*/)
{
	return S_OK;
}

STDMETHODIMP CThreadPoolService::OnShutdown()
{
	RETURN_IF_FAILED(Stop());
	boost::lock_guard<boost::mutex> lock(m_mutex);
	for (size_t i = 0; i < m_threads.size(); i++)
	{
		CComQIPtr<IPluginSupportNotifications> p = m_threads[i].m_T;
		RETURN_IF_FAILED(p->OnShutdown());
		RETURN_IF_FAILED(AtlUnadvise(m_threads[i].m_T, __uuidof(IThreadServiceEventSink), m_advices[i]));
	}

	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	return S_OK;
}

STDMETHODIMP CThreadPoolService::SetThreadCount(DWORD dwCount)
{
	RETURN_IF_FAILED(Stop());

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		for (size_t i = 0; i < m_threads.size(); i++)
		{
			m_threads[i].m_T->Join();
			RETURN_IF_FAILED(AtlUnadvise(m_threads[i].m_T, __uuidof(IThreadServiceEventSink), m_advices[i]));
		}

		m_threads.clear();
		m_advices.clear();

		for (size_t i = 0; i < dwCount; i++)
		{
			CComPtr<IThreadService> pThreadService;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThreadService, &pThreadService));
			CComPtr<IUnknown> pUnk;
			RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
			DWORD dwAdvice = 0;
			RETURN_IF_FAILED(AtlAdvise(pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &dwAdvice));
			m_threads.push_back(pThreadService);
			m_advices.push_back(dwAdvice);
		}
	}

	return S_OK;
}

STDMETHODIMP CThreadPoolService::Start()
{
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);

		if (m_bRunning)
			return S_OK;

		m_bRunning = true;
		m_bWaitingForStop = FALSE;
	}
	return S_OK;
}

STDMETHODIMP CThreadPoolService::Stop()
{
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_bWaitingForStop = TRUE;

		if (m_pQueueThread && m_pQueueThread->joinable())
		{
			m_pQueueThread->detach();
			m_pQueueThread.reset();
		}
	}

	return S_OK;
}

STDMETHODIMP CThreadPoolService::AddTask(IVariantObject* pVariantObject)
{
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		if (!m_bRunning)
			return E_PENDING;
		m_taskQueue.push(CAdapt<CComPtr<IVariantObject> >(pVariantObject));
		if (!m_pQueueThread)
			m_pQueueThread = make_shared<thread>(&CThreadPoolService::Run, this);
	}

	return S_OK;
}

STDMETHODIMP CThreadPoolService::Suspend()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_cSuspendRefs++;
	return S_OK;
}

STDMETHODIMP CThreadPoolService::Resume()
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_cSuspendRefs--;
	if (!m_cSuspendRefs)
	{
		if (!m_pQueueThread)
			m_pQueueThread = make_shared<thread>(&CThreadPoolService::Run, this);
	}
	return S_OK;
}

STDMETHODIMP CThreadPoolService::Run()
{
	CComPtr<IThreadPoolService> pGuard = this;
	shared_ptr<thread> pThisThread = m_pQueueThread;

	while (true)
	{
		Sleep(100);

		if (m_cSuspendRefs > 0)
			continue;

		while (true)
		{
			CComPtr<IVariantObject> pTaskObject;
			BOOL bRetry = FALSE;
			{
				boost::lock_guard<boost::mutex> lock(m_mutex);

				if (m_bWaitingForStop)
				{
					m_bWaitingForStop = FALSE;
					m_bRunning = FALSE;
					goto Exit;
				}

				if (m_taskQueue.empty())
					goto Exit;

				while (m_taskQueue.size())
				{
					vector<CAdapt<CComPtr<IThreadService> > > freeThreads(m_threads.size());
					{
						std::sort(m_threads.begin(), m_threads.end());
						std::sort(m_threadsInWork.begin(), m_threadsInWork.end());
						auto it = set_difference(m_threads.begin(), m_threads.end(), m_threadsInWork.begin(), m_threadsInWork.end(), freeThreads.begin());
						freeThreads.resize(it - freeThreads.begin());
					}

					if (freeThreads.empty())
					{ //no free threads
						bRetry = TRUE;
						break;
					}
					else
					{
						CComPtr<IThreadService> pThread;

						pThread = freeThreads[0];
						pTaskObject = m_taskQueue.front();
						m_taskQueue.pop();
						m_threadsInWork.push_back(pThread);
						m_threadsTaskContex[pThread] = pTaskObject;
						pThread->Run();
					}
				}
			}

			if (bRetry)
			{
				break;
			}
		}
	}

Exit:
	boost::lock_guard<boost::mutex> lock(m_mutex);
	if (m_pQueueThread && m_pQueueThread->joinable())
		m_pQueueThread->detach();
	m_pQueueThread.reset();
	return S_OK;
}

STDMETHODIMP CThreadPoolService::OnStart(IVariantObject *pResult)
{
	CComVariant v;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_THREAD_ID, &v));
	ATLASSERT(v.vt == VT_UNKNOWN);
	CComPtr<IUnknown> pUnk = v.punkVal;
	CComQIPtr<IThreadService> pThreadService = pUnk;

	{
		//boost::lock_guard<boost::mutex> lock(m_mutex);
		CComPtr<IVariantObject> pTaskContext = m_threadsTaskContex[pThreadService];
		RETURN_IF_FAILED(pTaskContext->CopyTo(pResult));
		m_threadsTaskContex.erase(pThreadService);
	}

	Fire_OnStart(pResult);
	return S_OK;
}

STDMETHODIMP CThreadPoolService::OnRun(IVariantObject *pResult)
{
	Fire_OnRun(pResult);
	return S_OK;
}

STDMETHODIMP CThreadPoolService::OnFinish(IVariantObject *pResult)
{
	Fire_OnFinish(pResult);
	CComVariant v;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_THREAD_ID, &v));
	ATLASSERT(v.vt == VT_UNKNOWN);
	CComPtr<IUnknown> pUnk = v.punkVal;
	CComQIPtr<IThreadService> pThreadService = pUnk;

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_threadsInWork.erase(remove(m_threadsInWork.begin(), m_threadsInWork.end(), pThreadService), m_threadsInWork.end());
	}

	return S_OK;
}

HRESULT CThreadPoolService::Fire_OnStart(IVariantObject *pResult)
{
	HRESULT hr = S_OK;
	CThreadPoolService* pThis = static_cast<CThreadPoolService*>(this);
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

HRESULT CThreadPoolService::Fire_OnFinish(IVariantObject *pResult)
{
	HRESULT hr = S_OK;
	CThreadPoolService* pThis = static_cast<CThreadPoolService*>(this);
	int cConnections = m_vec.GetSize();

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

HRESULT CThreadPoolService::Fire_OnRun(IVariantObject *pResult)
{
	HRESULT hr = S_OK;
	CThreadPoolService* pThis = static_cast<CThreadPoolService*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

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