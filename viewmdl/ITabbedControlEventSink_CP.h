#pragma once

#include "ViewMdl_i.h"

using namespace ATL;

template<class T>
class CProxyITabbedControlEventSink :
	public ATL::IConnectionPointImpl<T, &__uuidof(ITabbedControlEventSink)>
{
public:
	HRESULT Fire_OnClose(IControl* pControl)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			ITabbedControlEventSink * pConnection = static_cast<ITabbedControlEventSink*>(punkConnection.p);

			if (pConnection)
			{
				hr = pConnection->OnClose(pControl);
			}
		}
		return hr;
	}
};

