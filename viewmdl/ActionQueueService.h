// ActionQueueService.h : Declaration of the CActionQueueService

#pragma once
#include "resource.h"       // main symbols
#include "viewmdl_contract_i.h"
#include "ActionQueueServiceEventSink.h"
#include "ActionContext.h"

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>

using namespace ATL;

// CActionQueueService

class ATL_NO_VTABLE CActionQueueService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CActionQueueService, &CLSID_ActionQueueService>,
	public IActionQueueService,
	public IMsgHandler,
	public IInitializeWithControlImpl,
	public IPluginSupportNotifications,
	public IConnectionPointContainerImpl<CActionQueueService>,
	public ActionQueueServiceEventSink<CActionQueueService>
{
private:

public:
	CActionQueueService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CActionQueueService)
		COM_INTERFACE_ENTRY(IActionQueueService)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CActionQueueService)
		CONNECTION_POINT_ENTRY(__uuidof(IActionQueueServiceEvents))
	END_CONNECTION_POINT_MAP()

private:
	boost::mutex m_mutex;
	boost::mutex m_mutexThread;
	boost::condition_variable m_condition;
	std::queue<CActionContext> m_commandsToExecute;
	std::queue<CActionContext> m_commandsResult;

	CComPtr<IControl> m_pControl;
	CComPtr<IServiceProvider> m_pServiceProvider;

	static void ThreadProcStatic(void* pData);
	HRESULT ThreadProc();
	void ClearState();

	void OnBeforeCommandExecute();
	void OnAfterCommandExecute();

public:
	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult);
	STDMETHOD(QueueAction)(IAction* pAction);
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

private:

	class CBoolFLagSafeScope
	{
		bool m_bPrevValue;
		volatile bool* m_pbFlag;

	public:
		CBoolFLagSafeScope(volatile bool* pbFlag)
		{
			m_bPrevValue = *pbFlag;
			*pbFlag = TRUE;
			m_pbFlag = pbFlag;
		}

		~CBoolFLagSafeScope()
		{
			*m_pbFlag = m_bPrevValue;
			m_pbFlag = NULL;
		}
	};

	class CEventScope
	{
	private:
		HWND m_hWnd;
	public:
		CEventScope(HWND hWnd)
		{
			m_hWnd = hWnd;
			::PostMessage(m_hWnd, WM_COMMAND_START, 0, 0);
		}

		~CEventScope()
		{
			::PostMessage(m_hWnd, WM_COMMAND_END, 0, 0);
		}
	};
};

OBJECT_ENTRY_AUTO(__uuidof(ActionQueueService), CActionQueueService)
