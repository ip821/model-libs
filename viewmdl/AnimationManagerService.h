#pragma once

#include <UIAnimation.h>
#include "ViewMdl_i.h"

using namespace ATL;

// CAnimationManagerService

class ATL_NO_VTABLE CAnimationManagerService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAnimationManagerService, &CLSID_AnimationManagerService>,
	public IAnimationManagerServiceInternal,
	public IPluginSupportNotifications
{
public:
	CAnimationManagerService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CAnimationManagerService)
		COM_INTERFACE_ENTRY(IAnimationManagerService)
		COM_INTERFACE_ENTRY(IAnimationManagerServiceInternal)
	END_COM_MAP()

private:
public:
	STDMETHOD(CreateAnimation)(REFGUID clsidAnimationType, IAnimation** ppAnimation);
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();
};

OBJECT_ENTRY_AUTO(__uuidof(AnimationManagerService), CAnimationManagerService)
