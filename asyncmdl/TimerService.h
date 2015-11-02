#pragma once

#include "resource.h"       // main symbols
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "Plugins.h"
#include "NotificationServices_i.h"

using namespace ATL;

// CTimerService

class ATL_NO_VTABLE CTimerService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimerService, &CLSID_TimerService>,
	public IInitializeWithControlImpl,
	public ITimerService,
	public IPluginSupportNotifications,
	public IMsgHandler,
	public IConnectionPointContainerImpl<CTimerService>,
	public IConnectionPointImpl<CTimerService, &__uuidof(ITimerServiceEventSink)>
{
public:
	CTimerService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTimerService)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ITimerService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CTimerService)
		CONNECTION_POINT_ENTRY(__uuidof(ITimerServiceEventSink))
	END_CONNECTION_POINT_MAP()

private:

	UINT m_uiTimeout = 0;
	HRESULT Fire_OnTimer();

public:

	STDMETHOD(StartTimer)(UINT uiMilliseconds);
	STDMETHOD(ResumeTimer)();
	STDMETHOD(StopTimer)();

	STDMETHOD(OnUpdateTimer)();

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();
	
	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult);
};

OBJECT_ENTRY_AUTO(CLSID_TimerService, CTimerService)
