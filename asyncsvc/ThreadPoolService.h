// ThreadPoolService.h : Declaration of the CThreadPoolService

#pragma once
#include "resource.h"       // main symbols
#include "objmdl_contract_i.h"
#include "NotificationServices_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP::AsyncServices;

// CThreadPoolService

class ATL_NO_VTABLE CThreadPoolService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CThreadPoolService, &CLSID_ThreadPoolService>,
	public IThreadPoolService,
	public IInitializeWithControlImpl,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IConnectionPointContainerImpl<CThreadPoolService>,
	public IConnectionPointImpl<CThreadPoolService, &__uuidof(IThreadServiceEventSink)>
{
public:
	CThreadPoolService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_THREADPOOLSERVICE)

	BEGIN_COM_MAP(CThreadPoolService)
		COM_INTERFACE_ENTRY(IThreadPoolService)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CThreadPoolService)
		CONNECTION_POINT_ENTRY(__uuidof(IThreadServiceEventSink))
	END_CONNECTION_POINT_MAP()

private:
	vector<CComPtr<IThreadService>> m_threads;
	vector<CComPtr<IThreadService>> m_threadsInWork;
	map<CComPtr<IThreadService>, CComPtr<IVariantObject>> m_threadsTaskContex;

	vector<DWORD> m_advices;
	queue<CComPtr<IVariantObject>> m_taskQueue;
	BOOL m_bWaitingForStop = FALSE;
	BOOL m_bRunning = FALSE;
	boost::mutex m_mutex;
	volatile int m_cSuspendRefs = 0;
	atomic<HANDLE> m_handle = 0;

	HRESULT Fire_OnStart(IVariantObject *pResult);
	HRESULT Fire_OnRun(IVariantObject *pResult);
	HRESULT Fire_OnFinish(IVariantObject *pResult);

	void JoinAndStop(bool bJoin = true);
	void StartQueueThreadIfNecessary();

	static unsigned __stdcall ThreadProc(void* pThis);

public:
	STDMETHOD(Run)();

	STDMETHOD(SetThreadCount)(DWORD dwCount);
	STDMETHOD(Start)();
	STDMETHOD(Stop)();
	STDMETHOD(AddTask)(IVariantObject* pVariantObject);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(Suspend)();
	STDMETHOD(Resume)();
};

OBJECT_ENTRY_AUTO(__uuidof(ThreadPoolService), CThreadPoolService)
