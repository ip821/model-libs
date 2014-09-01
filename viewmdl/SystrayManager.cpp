// SystrayManager.cpp : Implementation of CSystrayManager

#include "stdafx.h"
#include "SystrayManager.h"
#include "SystrayWindow.h"

// CSystrayManager

STDMETHODIMP CSystrayManager::CreateSystrayWindow(HWND hwndParent, ISystrayWindow** ppWindow)
{
	CHECK_E_POINTER(hwndParent);
	CHECK_E_POINTER(ppWindow);

	CComPtr<CSystrayWindow> pWnd;
	RETURN_IF_FAILED(CSystrayWindow::_CreatorClass::CreateInstance(NULL, IID_ISystrayWindow, (LPVOID*)&pWnd));
	RETURN_IF_FAILED(pWnd->Create(hwndParent));
	return pWnd->QueryInterface(IID_ISystrayWindow, (LPVOID*)ppWindow);
}