// CommandSupport.cpp : Implementation of CCommandSupport

#include "stdafx.h"
#include "CommandSupport.h"


// CCommandSupport

STDMETHODIMP CCommandSupport::AddToolbarCommand(REFGUID guidCommand, ICommand2* pCommand)
{
	CHECK_E_POINTER(pCommand);
	return InsertToolbarCommand(guidCommand, m_toolBar.GetButtonCount(), pCommand);
}

STDMETHODIMP CCommandSupport::InsertToolbarCommand(REFGUID guidCommand, UINT ulIndex, ICommand2* pCommand2)
{
	CHECK_E_POINTER(pCommand2);
	if (!m_toolBar)
		return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

	HBITMAP hBitmap = { 0 };
	HRESULT hr = pCommand2->GetHBITMAP(guidCommand, &hBitmap);

	if (hr != S_OK && hr != E_NOTIMPL)
		return hr;

	int iIndex = m_imageList.Add(hBitmap, (COLORREF)0);

	CComBSTR bstr;
	RETURN_IF_FAILED(pCommand2->GetCommandText(guidCommand, &bstr));
	LPWSTR lpw = bstr;

	++m_CmdIdCounter;
	RegisterCommand(pCommand2, guidCommand, m_CmdIdCounter);

	TBBUTTON tb = { 0 };
	tb.iBitmap = iIndex;
	tb.idCommand = m_CmdIdCounter;
	tb.fsState = TBSTATE_ENABLED;
	tb.fsStyle = BTNS_BUTTON;
	tb.dwData = 0;
	tb.iString = (INT_PTR)lpw;
	BOOL bRes = m_toolBar.InsertButton(ulIndex, &tb);
	return bRes ? S_OK : E_FAIL;
}

STDMETHODIMP CCommandSupport::InsertMenuCommand(REFGUID guidParentCommand, REFGUID guidCommand, UINT ulIndex, ICommand* pCommand)
{
	CHECK_E_POINTER(pCommand);
	return InsertCommandToMenu(guidParentCommand, guidCommand, ulIndex, pCommand);
}

STDMETHODIMP CCommandSupport::AddMenuCommand(REFGUID guidParentCommand, REFGUID guidCommand, ICommand* pCommand)
{
	CHECK_E_POINTER(pCommand);
	return InsertCommandToMenu(guidParentCommand, guidCommand, 32767, pCommand);
}

STDMETHODIMP CCommandSupport::EnableCommandInternal(REFGUID guidCommand, BOOL bEnable)
{
	auto itGuid = m_InstalledCommandsGuidToIdMap.find(guidCommand);
	if (itGuid == m_InstalledCommandsGuidToIdMap.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	for (auto it = itGuid->second.begin(); it != itGuid->second.end(); it++)
	{
		EnableMenuItem(m_menu, *it, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_DISABLED));
		if (m_toolBar)
			m_toolBar.EnableButton(*it, bEnable);
	}

	return S_OK;
}

STDMETHODIMP CCommandSupport::InsertCommandToMenu(REFGUID guidParentCommand, REFGUID guidCommand, UINT ulIndex, ICommand* pCommand)
{
	CHECK_E_POINTER(pCommand);
	if (!m_menu)
		return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

	HRESULT hr = InstallStdParentMenuItemsIfNeed(guidParentCommand);
	if (FAILED(hr))
		return hr;

	UINT parentId = 0;
	for (auto it = m_InstalledCommandsMap.begin(); it != m_InstalledCommandsMap.end(); it++)
	{
		GUID guidId = m_InstalledCommandsIdToGuidMap[it->first];
		if (guidId == guidParentCommand)
		{
			parentId = it->first;
			break;
		}
	}

	CComBSTR bstr;
	pCommand->GetCommandText(guidCommand, &bstr);
	LPWSTR lpw = bstr;

	++m_CmdIdCounter;

	HMENU hMenu = m_menu;
	BOOL bRes = FALSE;

	if (parentId)
	{
		MENUITEMINFO mii = { 0 };
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_SUBMENU;

		bRes = GetMenuItemInfo(m_menu, parentId, FALSE, &mii);
		if (!mii.hSubMenu)
		{
			HMENU hSubMenu = CreateMenu();
			mii.hSubMenu = hSubMenu;
			mii.fMask = MIIM_SUBMENU;
			bRes = SetMenuItemInfo(m_menu, parentId, FALSE, &mii);
		}

		hMenu = mii.hSubMenu;
	}

	bRes = InsertMenu(hMenu, ulIndex, MF_BYPOSITION | MF_STRING, m_CmdIdCounter, lpw);

	CComQIPtr<ICommand2> pCommand2 = pCommand;
	if (pCommand2)
	{
		HBITMAP hBitmap = { 0 };
		if (pCommand2->GetHBITMAP(guidCommand, &hBitmap) == S_OK)
		{
			MENUITEMINFO menuInfo = { 0 };
			menuInfo.cbSize = sizeof(MENUITEMINFO);
			menuInfo.fMask = MIIM_BITMAP;

			if (m_pBitmapService)
			{
				//TODO HBITMAP leak
				//fix when implement menu item remove
				RETURN_IF_FAILED(m_pBitmapService->MakeBitMapTransparent(hBitmap, &menuInfo.hbmpItem));
			}
			else
			{
				menuInfo.hbmpItem = hBitmap;
			}

			bRes = SetMenuItemInfo(hMenu, m_CmdIdCounter, FALSE, &menuInfo);
		}
	}

	ATLASSERT(hr == S_OK);
	RegisterCommand(pCommand, guidCommand, m_CmdIdCounter);
	return S_OK;
}

STDMETHODIMP CCommandSupport::RegisterCommand(ICommand* pCommand, REFGUID gCommand, UINT cmdId)
{
	CHECK_E_POINTER(pCommand);
	m_InstalledCommandsMap[m_CmdIdCounter] = pCommand;
	m_InstalledCommandsGuidToIdMap[gCommand].push_back(cmdId);
	m_InstalledCommandsIdToGuidMap[cmdId] = gCommand;

	CComQIPtr<IAcceleratorSupport> pAccelSupport = pCommand;
	if (pAccelSupport)
	{
		TACCEL accel = { 0 };
		RETURN_IF_FAILED(pAccelSupport->GetAccelerator(gCommand, &accel));
		accel.cmd = (WORD)cmdId;
		m_accels.push_back(accel);
	}

	if (m_accels.size())
	{
		if (m_hAccel)
			::DestroyAcceleratorTable(m_hAccel);
		m_hAccel = CreateAcceleratorTable((LPACCEL)&m_accels[0], static_cast<int>(m_accels.size()));
	}

	return S_OK;
}

STDMETHODIMP CCommandSupport::PreTranslateMessage(HWND hWnd, MSG *pMsg, BOOL *pbResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(pbResult);
#ifdef DEBUG
	auto hwnd1 = GetAncestor(pMsg->hwnd, GA_ROOT); hwnd1;
	auto hwnd2 = GetAncestor(hWnd, GA_ROOT); hwnd2;
#endif
	if (
		hWnd &&
		((pMsg->hwnd == hWnd) || (GetAncestor(pMsg->hwnd, GA_ROOT) == GetAncestor(hWnd, GA_ROOT))) &&
		m_hAccel &&
		::TranslateAccelerator(hWnd, m_hAccel, pMsg)
		)
		*pbResult = TRUE;
	return S_OK;
}

STDMETHODIMP CCommandSupport::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult)
{
	*bResult = ProcessWindowMessage(hWnd, uMsg, wParam, lParam, *plResult);
	return S_OK;
}

STDMETHODIMP CCommandSupport::SetVariantObject(IVariantObject* pVariantObject)
{
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CCommandSupport::SetColumnName(LPCTSTR lpszColumnName)
{
	m_strColumnName = lpszColumnName;
	return S_OK;
}

STDMETHODIMP CCommandSupport::QueueCommandExecution(GUID guidCommand, VARIANT* vParam)
{
	auto itGuid = m_InstalledCommandsGuidToIdMap.find(guidCommand);
	if (itGuid == m_InstalledCommandsGuidToIdMap.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	auto commandId = *(itGuid->second.begin());

	HWND hWnd = 0;
	RETURN_IF_FAILED(m_pControl->GetHWND(&hWnd));
	::PostMessage(hWnd, WM_COMMAND, commandId, (LPARAM)(PVOID)vParam);
	return S_OK;
}

LRESULT CCommandSupport::OnCommandClick(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	wNotifyCode;
	hWndCtl;

	bHandled = FALSE;
	auto it = m_InstalledCommandsMap.find(wID);
	if (it != m_InstalledCommandsMap.end())
	{
		auto guidCommand = m_InstalledCommandsIdToGuidMap[wID];
		CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = it->second;
		if (pInitializeWithVariantObject)
		{
			RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(m_pVariantObject));
		}
		CComQIPtr<IInitializeWithColumnName> pInitializeWithColumnName = it->second;
		if (pInitializeWithColumnName)
		{
			RETURN_IF_FAILED(pInitializeWithColumnName->SetColumnName(m_strColumnName));
		}

		Fire_OnBeforeCommandInvoke(guidCommand, wNotifyCode, it->second);

		HRESULT hr = it->second->Invoke(guidCommand);
		if (hr == S_OK)
		{
			Fire_OnCommandInvoke(guidCommand);
		}

		if (pInitializeWithVariantObject)
		{
			RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(NULL));
		}

		if (pInitializeWithColumnName)
		{
			RETURN_IF_FAILED(pInitializeWithColumnName->SetColumnName(NULL));
		}

		if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))
		{
			MessageBox(GetActiveWindow(), _com_error(hr).ErrorMessage(), L"Error", MB_ICONERROR);
		}
		bHandled = TRUE;
	}
	return 0;
}

STDMETHODIMP CCommandSupport::UninstallAll()
{
	if (m_hAccel)
		::DestroyAcceleratorTable(m_hAccel);

	m_InstalledCommandsMap.clear();
	m_InstalledCommandsIdToGuidMap.clear();
	m_InstalledCommandsGuidToIdMap.clear();
	return S_OK;
}

STDMETHODIMP CCommandSupport::InstallStdParentMenuItemsIfNeed(REFGUID guidParentCommand)
{
	if (!m_bInstallStdCommands)
		return S_OK;

	if (guidParentCommand == COMMAND_TOOLS && m_InstalledCommandsGuidToIdMap.find(COMMAND_TOOLS) == m_InstalledCommandsGuidToIdMap.end())
	{
		CComPtr<ICommand> pCommand;
		HRESULT hr = CToolsCommand::_CreatorClass::CreateInstance(NULL, IID_ICommand, (LPVOID*)&pCommand);
		if (FAILED(hr))
			return hr;
		hr = pCommand->InstallMenu(this);
		if (FAILED(hr))
			return hr;
	}

	if (guidParentCommand == COMMAND_VIEW && m_InstalledCommandsGuidToIdMap.find(COMMAND_VIEW) == m_InstalledCommandsGuidToIdMap.end())
	{
		CComPtr<ICommand> pCommand;
		HRESULT hr = CViewCommand::_CreatorClass::CreateInstance(NULL, IID_ICommand, (LPVOID*)&pCommand);
		if (FAILED(hr))
			return hr;
		hr = pCommand->InstallMenu(this);
		if (FAILED(hr))
			return hr;
	}

	if (guidParentCommand == COMMAND_HELP && m_InstalledCommandsGuidToIdMap.find(COMMAND_HELP) == m_InstalledCommandsGuidToIdMap.end())
	{
		CComPtr<ICommand> pCommand;
		HRESULT hr = CHelpCommand::_CreatorClass::CreateInstance(NULL, IID_ICommand, (LPVOID*)&pCommand);
		if (FAILED(hr))
			return hr;
		hr = pCommand->InstallMenu(this);
		if (FAILED(hr))
			return hr;
	}

	return S_OK;
}

STDMETHODIMP CCommandSupport::SetMenu(HMENU hMenu)
{
	m_menu = hMenu;
	return S_OK;
}

STDMETHODIMP CCommandSupport::GetHWND(HWND* hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWndToolBar;
	return S_OK;
}

STDMETHODIMP CCommandSupport::SetToolbar(HWND hToolbar)
{
	m_toolBar = hToolbar;
	m_hWndToolBar = hToolbar;
	return S_OK;
}

STDMETHODIMP CCommandSupport::SetBitmapService(IBitmapService* pBitmapService)
{
	CHECK_E_POINTER(pBitmapService);
	m_pBitmapService = pBitmapService;
	return S_OK;
}

STDMETHODIMP CCommandSupport::GetImageList(HIMAGELIST* phImageList)
{
	CHECK_E_POINTER(phImageList);
	*phImageList = m_imageList;
	return S_OK;
}

STDMETHODIMP CCommandSupport::InstallCommand(ICommand* pCommand)
{
	CHECK_E_POINTER(pCommand);
	if (m_menu)
	{
		RETURN_IF_FAILED(pCommand->InstallMenu(this));
	}

	CComQIPtr<ICommand2> pCommand2 = pCommand;
	if (pCommand2 && m_toolBar)
	{
		return pCommand2->InstallToolbar(this);
	}
	return S_OK;
}

STDMETHODIMP CCommandSupport::InstallCommands(IPluginSupport* pPluginSupport)
{
	CHECK_E_POINTER(pPluginSupport);
	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(pPluginSupport->GetPlugins(&pObjectArray));

	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for (UINT i = 0; i < uiCount; i++)
	{
		CComPtr<ICommand> pCommand;
		HRESULT hr = pObjectArray->GetAt(i, IID_ICommand, (LPVOID*)&pCommand);
		if (hr != S_OK && hr != E_NOINTERFACE)
			return hr;

		if (hr == S_OK)
		{
			hr = InstallCommand(pCommand);
			ATLASSERT(hr == S_OK || hr == E_NOTIMPL);
		}
	}

	return S_OK;
}

STDMETHODIMP CCommandSupport::OnIdle(BOOL* bResult)
{
	UNREFERENCED_PARAMETER(bResult);
	for (auto it = m_InstalledCommandsGuidToIdMap.begin(); it != m_InstalledCommandsGuidToIdMap.end(); it++)
	{
		auto itCommandId = it->second.begin();
		{
			GUID gCommand = m_InstalledCommandsIdToGuidMap[*itCommandId];

			BOOL bEnabled = TRUE;
			auto itState = m_commandState.find(gCommand);
			if (itState != m_commandState.end())
			{
				bEnabled = m_commandState[gCommand];
			}

			if (bEnabled)
			{
				CComPtr<ICommand> pCommand = m_InstalledCommandsMap[*itCommandId];
				CComQIPtr<ICommand2> pCommand2 = pCommand;
				if (pCommand && pCommand2)
				{
					CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = pCommand2;
					if (pInitializeWithVariantObject)
					{
						RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(m_pVariantObject));
					}

					pCommand2->GetEnabled(gCommand, &bEnabled);
				}
			}

			for (auto itId = it->second.begin(); itId != it->second.end(); itId++)
			{
				RETURN_IF_FAILED(EnableCommandInternal(gCommand, bEnabled));
			}
		}
	}
	return S_OK;
}

STDMETHODIMP CCommandSupport::EnableStdCommands(BOOL bEnable)
{
	m_bInstallStdCommands = bEnable;
	return S_OK;
}

STDMETHODIMP CCommandSupport::EnableCommand(REFGUID guidCommand, BOOL bEnable)
{
	m_commandState[guidCommand] = bEnable;
	OnIdle(NULL);
	return S_OK;
}
