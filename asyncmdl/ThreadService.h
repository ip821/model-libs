// ThreadService.h : Declaration of the CThreadService

#pragma once
#include "resource.h"       // main symbols
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "NotificationServices_i.h"

using namespace ATL;
using namespace std;
using namespace IP::AsyncServices;

// CThreadService

class ATL_NO_VTABLE CThreadService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CThreadService, &CLSID_ThreadService>,
	public IThreadService,
	public IMsgHandler,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public ITimerServiceEventSink,
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
	CComQIPtr<IThreadService> g_guard;
	CComPtr<ITimerService> m_pTimerService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	DWORD m_dwAdvice = 0;
	HRESULT m_hr = S_OK;
	CComPtr<IVariantObject> m_pResult;

	void OnRun();
	void OnStop();
	boost::mutex m_mutex;
	volatile HANDLE m_handle = 0;
	void JoinAndStop(bool bJoin = true);

	HRESULT Fire_OnStart();
	HRESULT Fire_OnRun();
	HRESULT Fire_OnFinish();
	HRESULT Fire_OnFinishInternal();

	static unsigned __stdcall ThreadProc(void* pThis);

protected:

public:

	void RunInternal();
	void StopInternal();
	void StartInternal();

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();
	STDMETHOD(OnTimer)(ITimerService* pTimerService);
	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult);
	STDMETHOD(SetThreadContext)(IVariantObject* pVariantObject);
	STDMETHOD(Run)();
	STDMETHOD(SetTimerService)(GUID gServiceId);

	STDMETHOD(Join)();
};

OBJECT_ENTRY_AUTO(__uuidof(ThreadService), CThreadService)
