#include "stdafx.h"
#include "AnimationManagerService.h"

STDMETHODIMP CAnimationManagerService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	return S_OK;
}

STDMETHODIMP CAnimationManagerService::OnShutdown()
{
	return S_OK;
}

STDMETHODIMP CAnimationManagerService::CreateAnimation(REFGUID clsidAnimationType, IAnimation** ppAnimation)
{
	CHECK_E_POINTER(ppAnimation);
	RETURN_IF_FAILED(HrCoCreateInstance(clsidAnimationType, ppAnimation));
	return S_OK;
}
