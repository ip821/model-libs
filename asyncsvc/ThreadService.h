// ThreadService.h : Declaration of the CThreadService

#pragma once
#include "resource.h"       // main symbols
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "NotificationServices_i.h"
#include "ThreadOperation.h"

using namespace ATL;

// CThreadService

class ATL_NO_VTABLE CThreadService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CThreadService, &CLSID_ThreadService>,
	public IThreadService,
	public IMsgHandler,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public ITimerServiceEventSink,
	public CThreadOperation,
	public IConnectionPointContainerImpl<CThreadService>,
	public IConnectionPointImpl<CThreadService, &__uuidof(IThreadServiceEventSink)>
{
public:
	CThreadService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CThreadService)
		COM_INTERFACE_ENTRY(IThreadService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(ITimerServiceEventSink)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CThreadService)
		CONNECTION_POINT_ENTRY(__uuidof(IThreadServiceEventSink))
	END_CONNECTION_POINT_MAP()

private:
	CComPtr<ITimerService> m_pTimerService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	DWORD m_dwAdvice = 0;
	HRESULT m_hr = S_OK;
	CComPtr<IVariantObject> m_pResult;

	HRESULT Fire_OnStart();
	HRESULT Fire_OnRun();
	HRESULT Fire_OnFinish();
	HRESULT Fire_OnFinishInternal();
protected:

	virtual void OnRun();
	virtual void OnStop();

public:
	
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();
	STDMETHOD(OnTimer)();
	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult);
	STDMETHOD(SetThreadContext)(IVariantObject* pVariantObject);
	STDMETHOD(Run)();
	STDMETHOD(SetTimerService)(GUID gServiceId);

	STDMETHOD(Join)();
};

OBJECT_ENTRY_AUTO(__uuidof(ThreadService), CThreadService)
