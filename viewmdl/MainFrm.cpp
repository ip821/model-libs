// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "mainfrm.h"

HRESULT CMainFrame::FinalConstruct()
{
	RETURN_IF_FAILED(HrGetPluginManager(&m_pPluginManager));
	RETURN_IF_FAILED(CPluginSupport::_CreatorClass::CreateInstance(NULL, IID_IPluginSupport, (LPVOID*)&m_pPluginSupport));
	RETURN_IF_FAILED(CCommandSupport::_CreatorClass::CreateInstance(NULL, IID_ICommandSupport, (LPVOID*)&m_pCommandSupport));
	RETURN_IF_FAILED(CStatusBarSupport::_CreatorClass::CreateInstance(NULL, IID_IStatusBar, (LPVOID*)&m_pStatusBarSupport));
	return S_OK;
}

STDMETHODIMP CMainFrame::SetFlags(MainWindowFlags flags)
{
	m_flags = flags;
	return S_OK;
}

LRESULT CMainFrame::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	if (m_pCommandSupport)
	{
		LRESULT lResult = 0;
		m_pCommandSupport->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bHandled);
		if (bHandled)
			return TRUE;
	}

	if (m_pPluginSupport)
	{
		CComQIPtr<IMsgHandler> pMsgHandler = m_pPluginSupport;
		if (pMsgHandler)
		{
			LRESULT lResult = 0;
			BOOL bResult = FALSE;
			ASSERT_IF_FAILED(pMsgHandler->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult));
			if (bResult)
				return bResult;
		}
	}
	return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (!pMsg)
		return FALSE;

	if (m_pStatusBarSupport)
	{
		BOOL bResult = FALSE;
		if (FAILED(m_pStatusBarSupport->PreTranslateMessage(pMsg, &bResult)))
		{
			ATLASSERT(FALSE);
			return FALSE;
		}

		if(bResult)
			return bResult;
	}

	if(m_pContainerControl)
	{
		BOOL bResult = FALSE;
		CComQIPtr<IControl> pControl = m_pContainerControl;
		if(pControl)
			pControl->PreTranslateMessage(pMsg, &bResult);

		if(bResult)
			return bResult;
	}

	if (m_pPluginSupport)
	{
		CComPtr<IObjArray> pObjectArray;
		if (FAILED(m_pPluginSupport->GetPlugins(&pObjectArray)))
		{
			ATLASSERT(FALSE);
			return FALSE;
		}

		UINT cb = 0;
		if (FAILED(pObjectArray->GetCount(&cb)))
		{
			ATLASSERT(FALSE);
			return FALSE;
		}

		for (UINT i = 0; i < cb; i++)
		{
			CComPtr<IMsgFilter> pMsgFilter;
			HRESULT hr = pObjectArray->GetAt(i, IID_IMsgFilter, (LPVOID*)&pMsgFilter);
			if (hr == E_NOINTERFACE)
				continue;

			if (pMsgFilter)
			{
				BOOL bResult = FALSE;
				pMsgFilter->PreTranslateMessage(pMsg, &bResult);
				if (bResult)
					return bResult;
			}
		}
	}

	if((pMsg->hwnd == m_hWnd || IsChild(pMsg->hwnd)) && IsDialogMessage(pMsg))
	{
		return TRUE;
	}

	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;


	return FALSE;
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	
	if (!m_pCommandSupport)
		return FALSE;

	BOOL bResult = FALSE;
	if (FAILED(m_pCommandSupport->OnIdle(&bResult)))
	{
		ATLASSERT(FALSE);
		return FALSE;
	}
	return bResult;
}

STDMETHODIMP CMainFrame::GetStatusBar(IStatusBar** ppStatusBar)
{
	CHECK_E_POINTER(ppStatusBar);
	return m_pStatusBarSupport->QueryInterface(IID_IStatusBar, (LPVOID*)ppStatusBar);
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// register object for message filtering and idle updates
	ATLASSERT(m_pMessageLoop != NULL);
	m_pMessageLoop->AddMessageFilter(this);
	m_pMessageLoop->AddIdleHandler(this);

	m_pPluginSupport->InitializePlugins(PNAMESP_HOSTFORM, PVIEWTYPE_COMMAND);
	m_pPluginSupport->InitializePlugins(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE);
	m_pPluginSupport->SetControl(this);
	m_pPluginSupport->OnInitializing();

	if (m_pContainerControl)
	{
		RETURN_IF_FAILED(InitContainerControl());
	}

	if ((m_flags & MainWindowFlags::MainWindowMenuBar) == 0)
	{
#pragma warning(suppress: 6387)
		SetMenu(NULL);
	}
	else
	{
		m_menuMain = GetMenu();
		m_menuMain.RemoveMenu(0, MF_BYPOSITION); //clear menu
	}

	if (m_flags & MainWindowFlags::MainWindowToolBar)
	{
		m_hWndToolBar = m_toolBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_TOOLBAR_STYLE);
		UIAddToolBar(m_hWndToolBar);
		HIMAGELIST hImageList = { 0 };
		m_pCommandSupport->GetImageList(&hImageList);
		m_toolBar.SetImageList(hImageList);
	}

	if (m_flags & MainWindowFlags::MainWindowStatusBar)
	{
		CreateSimpleStatusBar();
		m_statusBar.SubclassWindow(m_hWndStatusBar);
		m_pStatusBarSupport->SetHWND(m_hWndStatusBar);
	}

	CComPtr<IBitmapService> pBitmapService;
	if(SUCCEEDED(m_pPluginSupport->QueryService(SERVICE_BITMAP_SERVICE, IID_IBitmapService, (LPVOID*)&pBitmapService)))
	{
		RETURN_IF_FAILED(m_pCommandSupport->SetBitmapService(pBitmapService));
	}

	RETURN_IF_FAILED(m_pCommandSupport->SetControl(this));
	if (m_menuMain)
	{
		RETURN_IF_FAILED(m_pCommandSupport->SetMenu(m_menuMain));
	}

	if (m_hWndToolBar)
	{
		RETURN_IF_FAILED(m_pCommandSupport->SetToolbar(m_hWndToolBar));
	}

	RETURN_IF_FAILED(m_pCommandSupport->InstallCommands(m_pPluginSupport));

	if (m_pSettings)
	{
		RETURN_IF_FAILED(Load(m_pSettings));
	}

	m_pPluginSupport->OnInitialized();
	m_pPluginSupport->OnInitCompleted();

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	ATLASSERT(m_pMessageLoop != NULL);
    m_pMessageLoop->RemoveMessageFilter(this);
    m_pMessageLoop->RemoveIdleHandler(this);

    GetWindowPlacement(&m_placement);
    GetWindowRect(&m_rectForSettingsSave);

    if (m_pSettings)
    {
        RETURN_IF_FAILED(Save(m_pSettings));
    }

    Fire_OnDestroy();

	CComQIPtr<IPluginSupportNotifications> pPluginSupportNotifications = m_pContainerControl;
	if (pPluginSupportNotifications)
	{
		RETURN_IF_FAILED(pPluginSupportNotifications->OnShutdown());
	}
	
    m_pPluginSupport->OnShutdown();
	m_pCommandSupport->UninstallAll();

	m_pSettings.Release();
	m_pPluginManager.Release();
	m_pStatusBarSupport.Release();
	m_pPluginSupport.Release();
	m_pCommandSupport.Release();
	m_pMessageLoop.Release();
	m_pContainerControl.Release();

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnWindowActivate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

STDMETHODIMP CMainFrame::GetMessageLoop(IMessageLoop** ppMessageLoop)
{
	CHECK_E_POINTER(ppMessageLoop);
	if(!m_pMessageLoop)
		return E_PENDING;

	return m_pMessageLoop->QueryInterface(IID_IMessageLoop, (LPVOID*)ppMessageLoop);
}

STDMETHODIMP CMainFrame::SetMessageLoop(IMessageLoop* pMessageLoop)
{
	CHECK_E_POINTER(pMessageLoop);
	m_pMessageLoop = pMessageLoop;
	return S_OK;
}

STDMETHODIMP CMainFrame::PreTranslateMessage(MSG* pMsg, BOOL* bResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(bResult);
	*bResult = PreTranslateMessage(pMsg);
	return S_OK;
}

STDMETHODIMP CMainFrame::OnIdle(BOOL *bResult)
{
	CHECK_E_POINTER(bResult);
	*bResult = OnIdle();
	return S_OK;
}

STDMETHODIMP CMainFrame::GetHWND(HWND* hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CMainFrame::CreateEx(HWND hWndParent, HWND* phWnd)
{
	CHECK_E_POINTER(phWnd);
	m_hWnd = __super::CreateEx(hWndParent);
	*phWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CMainFrame::ShowWindow(int cmd, BOOL* bResult)
{
	CHECK_E_POINTER(bResult);
	if(m_CmdShow)
		cmd = m_CmdShow;

	*bResult = __super::ShowWindow(cmd);
	return S_OK;
}

STDMETHODIMP CMainFrame::InitContainerControl()
{
	HWND hwndContainer = NULL;
	CComQIPtr<IControl> pControl = m_pContainerControl;
	if (!pControl)
		return E_NOINTERFACE;

	RETURN_IF_FAILED(pControl->GetHWND(&hwndContainer));
	if (hwndContainer)
		return S_OK;

	CComQIPtr<IInitializeWithControl> pInitializeWithControl = m_pContainerControl;
	if (pInitializeWithControl)
		RETURN_IF_FAILED(pInitializeWithControl->SetControl(this));

	RETURN_IF_FAILED(pControl->CreateEx(m_hWnd, &hwndContainer));

	m_hWndClient = hwndContainer;

	CComQIPtr<IInitializeWithSettings> pInit = m_pContainerControl;
	if (pInit)
	{
		RETURN_IF_FAILED(pInit->Load(m_pSettings));
	}

	CComQIPtr<IPluginSupportNotifications> pPluginSupportNotifications = m_pContainerControl;
	if (pPluginSupportNotifications)
	{
		RETURN_IF_FAILED(pPluginSupportNotifications->OnInitialized(m_pPluginSupport));
	}
	return S_OK;
}

STDMETHODIMP CMainFrame::SetContainerControl(IContainerControl* pContainerControl)
{
	CHECK_E_POINTER(pContainerControl);
	m_pContainerControl = pContainerControl;

	if (m_hWnd)
	{
		RETURN_IF_FAILED(InitContainerControl());
	}

	return S_OK;
}

STDMETHODIMP CMainFrame::GetMainMenu(IMenu** ppMenu)
{
	CHECK_E_POINTER(ppMenu);
	return QueryInterface(IID_IMenu, (LPVOID*)ppMenu);
}

STDMETHODIMP CMainFrame::GetToolbar(IToolbar** ppToolbar)
{
	UNREFERENCED_PARAMETER(ppToolbar);
	return QueryInterface(IID_IToolbar, (LPVOID*)ppToolbar);
}

STDMETHODIMP CMainFrame::GetContainerControl(IContainerControl** ppContainerControl)
{
	CHECK_E_POINTER(ppContainerControl);
	return m_pContainerControl->QueryInterface(IID_IContainerControl, (LPVOID*)ppContainerControl);
}

STDMETHODIMP CMainFrame::SetIcon(HICON hIcon, BOOL bLarge)
{
	CFrameWindowImpl::SetIcon(hIcon, bLarge);
	return S_OK;
}

STDMETHODIMP CMainFrame::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);

	if (!m_hWnd)
	{
		m_pSettings = pSettings;
		return S_OK;
	}

	CComPtr<ISettings> pSettingsWindow;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(L"MainWindow", &pSettingsWindow));

	RECT rc = {0};
	GetWindowRect(&rc);

	bool posChanged = false;
	CComVar v;
	if (SUCCEEDED(pSettingsWindow->GetVariantValue(SETTINGS_LEFT, &v)) && v.vt == VT_I4)
	{
		rc.left = v.intVal;
		posChanged = true;
	}
	v.Clear();
	if (SUCCEEDED(pSettingsWindow->GetVariantValue(SETTINGS_TOP, &v)) && v.vt == VT_I4)
	{
		rc.top = v.intVal;
		posChanged = true;
	}
	v.Clear();
	if (SUCCEEDED(pSettingsWindow->GetVariantValue(SETTINGS_RIGHT, &v)) && v.vt == VT_I4)
	{
		rc.right = v.intVal;
		posChanged = true;
	}
	v.Clear();
	if (SUCCEEDED(pSettingsWindow->GetVariantValue(SETTINGS_BOTTOM, &v)) && v.vt == VT_I4)
	{
		rc.bottom = v.intVal;
		posChanged = true;
	}

	CComVar vShowCmd;
	if (SUCCEEDED(pSettingsWindow->GetVariantValue(SETTINGS_SHOWCMD, &vShowCmd)) && vShowCmd.vt == VT_I4 && vShowCmd.intVal != SW_NORMAL)
	{
		m_CmdShow = vShowCmd.intVal;
	}
	
	if(posChanged)
	{
		MoveWindow(&rc);
	}

	RETURN_IF_FAILED(m_pPluginSupport->Load(pSettings));
	CComQIPtr<IInitializeWithSettings> pInit = m_pContainerControl;
	if (pInit)
	{
		RETURN_IF_FAILED(pInit->Load(pSettings));
	}

	return S_OK;
}

STDMETHODIMP CMainFrame::Save(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	RECT rc = m_rectForSettingsSave;

	{
		CComPtr<ISettings> pSettingsWindow;
		RETURN_IF_FAILED(pSettings->OpenSubSettings(L"MainWindow", &pSettingsWindow));
		RETURN_IF_FAILED(pSettingsWindow->SetVariantValue(SETTINGS_SHOWCMD, &CComVar((int)m_placement.showCmd)));
		if (m_placement.showCmd == SW_NORMAL)
		{
			RETURN_IF_FAILED(pSettingsWindow->SetVariantValue(SETTINGS_LEFT, &CComVar(rc.left)));
			RETURN_IF_FAILED(pSettingsWindow->SetVariantValue(SETTINGS_RIGHT, &CComVar(rc.right)));
			RETURN_IF_FAILED(pSettingsWindow->SetVariantValue(SETTINGS_TOP, &CComVar(rc.top)));
			RETURN_IF_FAILED(pSettingsWindow->SetVariantValue(SETTINGS_BOTTOM, &CComVar(rc.bottom)));
		}
	}

	CComQIPtr<IPersistSettings> pPersistSettings = m_pContainerControl;
	if (pPersistSettings)
	{
		RETURN_IF_FAILED(pPersistSettings->Save(pSettings));
	}
	return S_OK;
}

STDMETHODIMP CMainFrame::Reset(ISettings* pSettings)
{
	UNREFERENCED_PARAMETER(pSettings);
	return E_NOTIMPL;
}

void CMainFrame::UpdateLayout(BOOL bResizeBars = TRUE)
{
	RECT rect = { 0 };
	GetClientRect(&rect);

	UpdateBarsPosition(rect, bResizeBars);

	if (m_hWndClient != NULL)
	{
		CRect clientWindowRect;
		::GetWindowRect(m_hWndClient, &clientWindowRect);
		::SetWindowPos(
			m_hWndClient, 
			NULL, 
			rect.left, 
			rect.top, 
			rect.right - rect.left, 
			rect.bottom - rect.top,
			SWP_NOZORDER | SWP_NOACTIVATE
			);
	}
}

HRESULT CMainFrame::Fire_OnDestroy()
{
    CComPtr<IUnknown> pUnk;
    RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
    HRESULT hr = S_OK;
    CMainFrame* pThis = static_cast<CMainFrame*>(this);
    int cConnections = m_vec.GetSize();

    for (int iConnection = 0; iConnection < cConnections; iConnection++)
    {
        pThis->Lock();
        CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
        pThis->Unlock();

        IMainWindowEventSink* pConnection = static_cast<IMainWindowEventSink*>(punkConnection.p);

        if (pConnection)
        {
            hr = pConnection->OnDestroy();
        }
    }
    return hr;
}
