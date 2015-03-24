#include "stdafx.h"
#include "AccelerateDecelerateAnimation.h"

STDMETHODIMP CAccelerateDecelerateAnimation::SetParams(DOUBLE dwFrom, DOUBLE dwTo, DOUBLE dwDuration)
{
	dwFrom;
	dwTo;
	dwDuration;
	return S_OK;
}

STDMETHODIMP CAccelerateDecelerateAnimation::Run()
{
	return S_OK;
}

STDMETHODIMP CAccelerateDecelerateAnimation::GetCurrentIntValue(INT32* piVal)
{
	CHECK_E_POINTER(piVal);
	DOUBLE dblVal = 0;
	RETURN_IF_FAILED(GetCurrentValue(&dblVal));
	*piVal = (int)dblVal;
	return S_OK;
}

STDMETHODIMP CAccelerateDecelerateAnimation::GetCurrentValue(DOUBLE* pdblVal)
{
	CHECK_E_POINTER(pdblVal);
	return S_OK;
}

HRESULT CAccelerateDecelerateAnimation::Fire_OnAnimation()
{
	HRESULT hr = S_OK;
	CAccelerateDecelerateAnimation* pThis = static_cast<CAccelerateDecelerateAnimation*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		IAnimationEventSink * pConnection = static_cast<IAnimationEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnAnimation();
		}
	}
	return hr;
}