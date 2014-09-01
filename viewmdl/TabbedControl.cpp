// TabbedControl.cpp : Implementation of CTabbedControl

#include "stdafx.h"
#include "TabbedControl.h"


// CTabbedControl

STDMETHODIMP CTabbedControl::EnableCommands(BOOL bEnabled)
{
	m_bCommandsEnabled = bEnabled;
	return S_OK;
}

STDMETHODIMP CTabbedControl::Load(ISettings* pSettings)
{
	int iItemCount = m_tab.GetItemCount();
	for (int i = 0; i < iItemCount; i++)
	{
		CComPtr<IControl> pPageControl;
		RETURN_IF_FAILED(GetPage(i, &pPageControl));
		CComQIPtr<IInitializeWithSettings> pInit = pPageControl;
		if (pInit)
		{
			RETURN_IF_FAILED(pInit->Load(pSettings));
		}
	}
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP CTabbedControl::Save(ISettings* pSettings)
{
	if (!IsWindow())
		return S_OK;
	int iItemCount = m_tab.GetItemCount();
	for (int i = 0; i < iItemCount; i++)
	{
		CComPtr<IControl> pPageControl;
		RETURN_IF_FAILED(GetPage(i, &pPageControl));
		CComQIPtr<IPersistSettings> pInit = pPageControl;
		if (pInit)
		{
			RETURN_IF_FAILED(pInit->Save(pSettings));
		}
	}
	return S_OK;
}

STDMETHODIMP CTabbedControl::ActivatePage(IControl* pControl)
{
	CHECK_E_POINTER(pControl);
	HWND hwnd = 0;
	RETURN_IF_FAILED(pControl->GetHWND(&hwnd));

	int iItemCount = m_tab.GetItemCount();
	for (int i = 0; i < iItemCount; i++)
	{
		CComPtr<IControl> pPageControl;
		RETURN_IF_FAILED(GetPage(i, &pPageControl));
		HWND hwndPageControl = 0;
		RETURN_IF_FAILED(pPageControl->GetHWND(&hwndPageControl));
		if (hwndPageControl == hwnd)
		{
			SetActivePage(i);
			return S_OK;
		}
	}
	return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
}

STDMETHODIMP CTabbedControl::GetPageCount(DWORD* pdwCount)
{
	CHECK_E_POINTER(pdwCount);
	*pdwCount = m_tab.GetItemCount();
	return S_OK;
}

STDMETHODIMP CTabbedControl::GetPage(DWORD dwIndex, IControl** ppControl)
{
	CHECK_E_POINTER(ppControl);
	auto hWnd = GetPageHWND(dwIndex);
	auto it = m_pControls.find(hWnd);

	if (it == m_pControls.end())
	{
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}

	return it->second.m_T->QueryInterface(IID_IControl, (LPVOID*)ppControl);
}

STDMETHODIMP CTabbedControl::AddPage(IControl* pControl)
{
	CHECK_E_POINTER(pControl);
	HWND hwnd = NULL;
	HRESULT hr = pControl->CreateEx(m_hWnd, &hwnd);
	if(FAILED(hr))
		return hr;

	m_pControls[hwnd] = pControl;

	CComBSTR bstr = L"No caption";
	CComQIPtr<IControl2> pControl2 = pControl;
	if(pControl2)
	{
		RETURN_IF_FAILED(pControl2->GetText(&bstr));
	}
	__super::AddPage(hwnd, bstr);

	CComQIPtr<IInitializeWithSettings> pInit = pControl;
	if (pInit && m_pSettings)
	{
		RETURN_IF_FAILED(pInit->Load(m_pSettings));
	}

	CComQIPtr<IPluginSupportNotifications> pPluginSupportNotifications = pControl;
	if (pPluginSupportNotifications)
	{
		RETURN_IF_FAILED(pPluginSupportNotifications->OnInitialized(m_pServiceProvider));
	}

	return S_OK;
}

STDMETHODIMP CTabbedControl::UpdateControl(IControl* pControl)
{
	CHECK_E_POINTER(pControl);
	HWND hwnd = 0;
	RETURN_IF_FAILED(pControl->GetHWND(&hwnd));
	auto it = m_pControls.find(hwnd);
	if(it != m_pControls.end())
	{
		CComPtr<IControl> pControl = it->second;
		CComBSTR bstr = L"No caption";
		CComQIPtr<IControl2> pControl2 = pControl;
		if(pControl2)
		{
			RETURN_IF_FAILED(pControl2->GetText(&bstr));
			__super::SetPageTitle(PageIndexFromHwnd(hwnd), bstr);
		}
	}
	return S_OK;
}

STDMETHODIMP CTabbedControl::GetCurrentPage(IControl** ppControl)
{
	CHECK_E_POINTER(ppControl);
	auto iActivePage = GetActivePage();
	if(iActivePage == -1)
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	return GetPage(iActivePage, ppControl);
}

STDMETHODIMP CTabbedControl::RemovePage(IControl* pControl)
{
	CHECK_E_POINTER(pControl);
	HWND hwnd = 0;
	RETURN_IF_FAILED(pControl->GetHWND(&hwnd));
	auto it = m_pControls.find(hwnd);
	if(it != m_pControls.end())
	{
		CComPtr<IControl> pControl = it->second;
		CComQIPtr<IControl2> pControl2 = pControl;
		if(pControl2)
		{
			pControl2->OnDeactivate();
			pControl2->OnClose();
		}

		__super::RemovePage(PageIndexFromHwnd(it->first));
		Fire_OnClose(pControl);
		m_pControls.erase(it);
	}
	return S_OK;
}

STDMETHODIMP CTabbedControl::GetHWND(HWND* phWnd)
{
	CHECK_E_POINTER(phWnd);
	*phWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CTabbedControl::PreTranslateMessage(MSG* pMsg, BOOL* pbResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(pbResult);
	auto activePage = GetActivePage();
	if(activePage == -1)
		return S_OK;

	if(CTabViewImpl::PreTranslateMessage(pMsg))
	{
		*pbResult = TRUE;
		return S_OK;
	}

	bool bControl = (::GetKeyState(VK_CONTROL) < 0);
	if (m_bCommandsEnabled && pMsg->message == WM_KEYDOWN && pMsg->wParam == 0x57 && bControl) //CTRL-W
	{
		CTabViewImpl::RemovePage(activePage);
		*pbResult = TRUE;
		return S_OK;
	}

	auto hwnd = GetPageHWND(activePage);
	auto it = m_pControls.find(hwnd);
	if(it != m_pControls.end())
	{
		it->second.m_T->PreTranslateMessage(pMsg, pbResult);
	}
	return S_OK;
}

STDMETHODIMP CTabbedControl::CreateEx(HWND hWndParent, HWND* phWnd)
{
	m_hWnd = Create(hWndParent, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CONTROLPARENT);
	if(phWnd)
		*phWnd = m_hWnd;

	return S_OK;
}

void CTabbedControl::OnPageActivated(int nPage)
{
	if(nPage == -1)
		return;

	HWND hWnd = GetPageHWND(nPage);
	auto it = m_pControls.find(hWnd);
	if(it != m_pControls.end())
	{
		CComPtr<IControl> pControl = it->second;
		CComQIPtr<IControl2> pControl2 = pControl;
		if(pControl2)
			pControl2->OnActivate();
	}
	__super::OnPageActivated(nPage);
}

void CTabbedControl::OnContextMenu(int nPage, POINT pt)
{
	if (!m_bCommandsEnabled)
		return;

	if(GetActivePage() != nPage)
	{
		SetActivePage(nPage);
		OnPageActivated(nPage);
	}
	ClientToScreen(&pt);
	m_popupMenu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);
}

LRESULT CTabbedControl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_PluginSupport->InitializePlugins(CONTROL_TABCONTAINER, PVIEWTYPE_COMMAND);
	m_PluginSupport->SetControl(this);
	m_PluginSupport->OnInitialized();

	m_pCommandSupport->SetMenu(m_popupMenu);
	m_pCommandSupport->InstallCommands(m_PluginSupport);

	return __super::OnCreate(uMsg, wParam, lParam, bHandled);
}

HRESULT CTabbedControl::FinalConstruct()
{
	RETURN_IF_FAILED(HrGetPluginManager(&m_pPluginManager));
	RETURN_IF_FAILED(CPluginSupport::_CreatorClass::CreateInstance(NULL, IID_IPluginSupport, (LPVOID*)&m_PluginSupport));
	RETURN_IF_FAILED(CCommandSupport::_CreatorClass::CreateInstance(NULL, IID_ICommandSupport, (LPVOID*)&m_pCommandSupport));
	return S_OK;
}

LRESULT CTabbedControl::OnTabMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (!m_bCommandsEnabled)
		return 0;

	TCHITTESTINFO hti = { 0 };
	hti.pt.x = GET_X_LPARAM(lParam);
	hti.pt.y = GET_Y_LPARAM(lParam);
	int nItem = m_tab.HitTest(&hti);
	if(nItem != -1 && nItem != GetActivePage())
	{
		SetActivePage(nItem);
	}

	CComPtr<IControl> pControl;
	if(GetCurrentPage(&pControl) == S_OK)
	{
		RemovePage(pControl);
	}

	return 0;
}

STDMETHODIMP CTabbedControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	int iItemCount = m_tab.GetItemCount();
	for (int i = 0; i < iItemCount; i++)
	{
		CComPtr<IControl> pPageControl;
		RETURN_IF_FAILED(GetPage(i, &pPageControl));
		CComQIPtr<IPluginSupportNotifications> pInit = pPageControl;
		if (pInit)
		{
			RETURN_IF_FAILED(pInit->OnInitialized(pServiceProvider));
		}
	}
	m_pServiceProvider = pServiceProvider;
	return S_OK;
}

STDMETHODIMP CTabbedControl::OnShutdown()
{
	int iItemCount = m_tab.GetItemCount();
	for (int i = 0; i < iItemCount; i++)
	{
		CComPtr<IControl> pPageControl;
		RETURN_IF_FAILED(GetPage(i, &pPageControl));
		CComQIPtr<IPluginSupportNotifications> pInit = pPageControl;
		if (pInit)
		{
			RETURN_IF_FAILED(pInit->OnShutdown());
		}
	}
	m_pServiceProvider.Release();
	return S_OK;
}
