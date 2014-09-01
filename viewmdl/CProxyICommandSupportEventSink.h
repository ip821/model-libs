#pragma once

#include "ViewMdl_i.h"

using namespace ATL;

template<class T>
class CProxyICommandSupportEventSink : public ATL::IConnectionPointImpl<T, &__uuidof(ICommandSupportEventSink)>
{
public:
	HRESULT Fire_OnCommandInvoke(REFGUID guidCommand)
	{
		HRESULT hr = S_OK;
		T * pThis = static_cast<T *>(this);
		int cConnections = m_vec.GetSize();

		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			pThis->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			pThis->Unlock();

			ICommandSupportEventSink * pConnection = static_cast<ICommandSupportEventSink*>(punkConnection.p);

			if (pConnection)
			{
				hr = pConnection->OnCommandInvoke(guidCommand);
			}
		}
		return hr;
	}
};

