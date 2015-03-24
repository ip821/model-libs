#include "stdafx.h"
#include "AccelerateDecelerateAnimation.h"

HRESULT CAccelerateDecelerateAnimation::FinalConstruct()
{
	return S_OK;
}

void CAccelerateDecelerateAnimation::FinalRelease()
{
	m_pAnimationTimer->SetTimerEventHandler(nullptr);
	m_pAnimationTimer->SetTimerUpdateHandler(nullptr, UI_ANIMATION_IDLE_BEHAVIOR_DISABLE);
}

STDMETHODIMP CAccelerateDecelerateAnimation::SetParams(DOUBLE dblFrom, DOUBLE dblTo, DOUBLE dblDuration)
{
	m_dblFrom = dblFrom;
	m_dblTo = dblTo;
	m_dblDuration = dblDuration;
	return S_OK;
}

STDMETHODIMP CAccelerateDecelerateAnimation::Run()
{
	RETURN_IF_FAILED(CoCreateInstance(CLSID_UIAnimationManager, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAnimationManager), (LPVOID*)&m_pAnimationManager));
	RETURN_IF_FAILED(CoCreateInstance(CLSID_UIAnimationTimer, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAnimationTimer), (LPVOID*)&m_pAnimationTimer));
	RETURN_IF_FAILED(CoCreateInstance(CLSID_UIAnimationTransitionLibrary, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAnimationTransitionLibrary), (LPVOID*)&m_pAnimationTransitionLibrary));

	RETURN_IF_FAILED(m_pAnimationManager->CreateAnimationVariable(m_dblFrom, &m_pUIAnimationVariable));
	RETURN_IF_FAILED(m_pUIAnimationVariable->SetLowerBound(m_dblFrom < m_dblTo ? m_dblFrom : m_dblTo));
	RETURN_IF_FAILED(m_pUIAnimationVariable->SetUpperBound(m_dblTo > m_dblFrom ? m_dblTo : m_dblFrom));

	RETURN_IF_FAILED(m_pAnimationTimer->SetTimerEventHandler(this));
	CComQIPtr<IUIAnimationTimerUpdateHandler> pUIAnimationTimerUpdateHandler = m_pAnimationManager;
	RETURN_IF_FAILED(m_pAnimationTimer->SetTimerUpdateHandler(pUIAnimationTimerUpdateHandler, UI_ANIMATION_IDLE_BEHAVIOR_DISABLE));

	RETURN_IF_FAILED(m_pAnimationTransitionLibrary->CreateAccelerateDecelerateTransition(m_dblDuration, m_dblTo, 0.5, 0.5, &m_pUIAnimationTransition));
	RETURN_IF_FAILED(m_pAnimationManager->CreateStoryboard(&m_pUIAnimationStoryboard));
	RETURN_IF_FAILED(m_pUIAnimationStoryboard->AddTransition(m_pUIAnimationVariable, m_pUIAnimationTransition));

	UI_ANIMATION_SECONDS secondsNow;
	RETURN_IF_FAILED(m_pAnimationTimer->GetTime(&secondsNow));
	RETURN_IF_FAILED(m_pUIAnimationStoryboard->Schedule(secondsNow));
	return S_OK;
}

STDMETHODIMP CAccelerateDecelerateAnimation::OnPostUpdate()
{
	RETURN_IF_FAILED(Fire_OnAnimation());
	return S_OK;
}

STDMETHODIMP CAccelerateDecelerateAnimation::IsAnimationComplete(BOOL* pbComplete)
{
	CHECK_E_POINTER(pbComplete);
	DOUBLE dblCurrentValue = 0;
	RETURN_IF_FAILED(GetCurrentValue(&dblCurrentValue));
	DOUBLE dblFinalValue = 0;
	RETURN_IF_FAILED(m_pUIAnimationVariable->GetFinalValue(&dblFinalValue));
	*pbComplete = dblCurrentValue == dblFinalValue;
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
	RETURN_IF_FAILED(m_pUIAnimationVariable->GetValue(pdblVal));
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