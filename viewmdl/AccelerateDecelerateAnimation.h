#pragma once

#include <UIAnimation.h>
#include "ViewMdl_i.h"

using namespace ATL;

// CAccelerateDecelerateAnimation

class ATL_NO_VTABLE CAccelerateDecelerateAnimation :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAccelerateDecelerateAnimation, &CLSID_AccelerateDecelerateAnimation>,
	public IAnimation,
	public IInitializeWithControlImpl,
	public IMsgHandler,
	public IUIAnimationTimerEventHandler,
	public IPluginSupportNotifications,
	public IConnectionPointContainerImpl<CAccelerateDecelerateAnimation>,
	public IConnectionPointImpl<CAccelerateDecelerateAnimation, &__uuidof(IAnimationEventSink)>
{
public:
	CAccelerateDecelerateAnimation()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CAccelerateDecelerateAnimation)
		COM_INTERFACE_ENTRY(IAnimation)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IUIAnimationTimerEventHandler)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CAccelerateDecelerateAnimation)
		CONNECTION_POINT_ENTRY(__uuidof(IAnimationEventSink))
	END_CONNECTION_POINT_MAP()

private:
	CComPtr<IUIAnimationManager> m_pAnimationManager;
	CComPtr<IUIAnimationTimer> m_pAnimationTimer;
	CComPtr<IUIAnimationTransitionLibrary> m_pAnimationTransitionLibrary;
	CComPtr<IUIAnimationVariable> m_pUIAnimationVariable;
	CComPtr<IUIAnimationTransition> m_pUIAnimationTransition;
	CComPtr<IUIAnimationStoryboard> m_pUIAnimationStoryboard;
	DOUBLE m_dblFrom = 0;
	DOUBLE m_dblTo = 0;
	DOUBLE m_dblDuration = 0;
	HRESULT Fire_OnAnimation();
	STDMETHOD(OnPostUpdateInternal)();

public:
	METHOD_EMPTY(STDMETHOD(OnPreUpdate)());
	METHOD_EMPTY(STDMETHOD(OnRenderingTooSlow)(UINT32 framesPerSecond));
	STDMETHOD(OnPostUpdate)();

	STDMETHOD(SetParams)(DOUBLE dblFrom, DOUBLE dblTo, DOUBLE dblDuration);
	STDMETHOD(Run)();
	STDMETHOD(GetCurrentIntValue)(INT32* piVal);
	STDMETHOD(GetCurrentValue)(DOUBLE* pdblVal);
	STDMETHOD(IsAnimationComplete)(BOOL* pbComplete);
	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult);

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();
};

OBJECT_ENTRY_AUTO(__uuidof(AccelerateDecelerateAnimation), CAccelerateDecelerateAnimation)
