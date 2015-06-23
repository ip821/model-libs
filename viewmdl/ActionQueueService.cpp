#include "stdafx.h"
#include "ActionQueueService.h"

STDMETHODIMP CActionQueueService::QueueAction(IAction* pAction)
{
	CHECK_E_POINTER(pAction);

	CActionContext actionContext;
	actionContext.m_pAction = pAction;

	{
		boost::lock_guard<boost::mutex> guard(m_mutex);
		m_commandsToExecute.push(actionContext);
	}

	m_condition.notify_one();

	return S_OK;
}

STDMETHODIMP CActionQueueService::ProcessWindowMessage(HWND /*hWnd*/, UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, LRESULT* /*plResult*/, BOOL* bResult)
{
	if (uMsg == WM_COMMAND_START)
	{
		OnBeforeCommandExecute();
		*bResult = TRUE;
	}

	if (uMsg == WM_COMMAND_END)
	{
		OnAfterCommandExecute();
		*bResult = TRUE;
	}

	return S_OK;
}

void CActionQueueService::OnBeforeCommandExecute()
{
	CActionContext actionContext;
	{
		boost::lock_guard<boost::mutex> guard(m_mutex);
		actionContext = m_commandsResult.front();
	}
	Fire_OnBeforeCommandExecute(actionContext.m_pAction);
}

void CActionQueueService::OnAfterCommandExecute()
{
	CActionContext actionContext;
	{
		boost::lock_guard<boost::mutex> guard(m_mutex);
		actionContext = m_commandsResult.front();
		m_commandsResult.pop();
	}
	CComQIPtr<IAction2> pAction2 = actionContext.m_pAction;
	if (pAction2)
		pAction2->AfterExecute();
	Fire_OnAfterCommandExecute(actionContext.m_pAction);
}

HRESULT CActionQueueService::ThreadProc()
{
	RETURN_IF_FAILED(CoInitialize(NULL));

	while (true)
	{
		boost::unique_lock<boost::mutex> lock(m_mutexThread);
		m_condition.wait(lock);

		while (true)
		{
			{
				boost::lock_guard<boost::mutex> guard(m_mutex);
				bool isEmpty = m_commandsToExecute.empty();
				if (isEmpty)
					break;
			}

			CActionContext actionContext;

			{
				boost::lock_guard<boost::mutex> guard(m_mutex);
				actionContext = m_commandsToExecute.front();
				m_commandsToExecute.pop();
			}

			while (true)
			{
				{
					boost::lock_guard<boost::mutex> guard(m_mutex);
					m_commandsResult.push(actionContext);
				}

				CEventScope eventScope(m_hControlWnd);
				if (actionContext.m_pAction)
				{
					CComVar vResult;
					HRESULT hr = actionContext.m_pAction->Execute();
					if (FAILED(hr))
					{
						boost::lock_guard<boost::mutex> guard(m_mutex);
					}
					break;
				}
			}
		}
	}
	return S_OK;
}

void CActionQueueService::ThreadProcStatic(void* pData)
{
	CActionQueueService* pWorkerTask = (CActionQueueService*)pData;
	pWorkerTask->ThreadProc();
}

STDMETHODIMP CActionQueueService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	_beginthread(&CActionQueueService::ThreadProcStatic, 0, this);
	return S_OK;
}

STDMETHODIMP CActionQueueService::OnShutdown()
{
	m_pServiceProvider = NULL;
	m_pControl = NULL;
	return S_OK;
}
