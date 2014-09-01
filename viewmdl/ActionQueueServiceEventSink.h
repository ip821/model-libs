#pragma once

using namespace ATL;

template<class T>
class ActionQueueServiceEventSink : public ATL::IConnectionPointImpl<T, &__uuidof(IActionQueueServiceEvents)>
{
public:
	HRESULT Fire_OnBeforeCommandExecute(IAction* pAction)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IActionQueueServiceEvents * pConnection = static_cast<IActionQueueServiceEvents*>(punkConnection.p);

			if (pConnection)
			{
				hr = pConnection->OnBeforeActionExecution(pAction);
			}
		}
		return hr;
	}

	HRESULT Fire_OnAfterCommandExecute(IAction* pAction)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			IActionQueueServiceEvents * pConnection = static_cast<IActionQueueServiceEvents*>(punkConnection.p);

			if (pConnection)
			{
				hr = pConnection->OnAfterActionExecution(pAction);
			}
		}
		return hr;
	}
};

