// InfoControlService.cpp : Implementation of CInfoControlService

#include "stdafx.h"
#include "InfoControlService.h"
#include "TabbedControl.h"
#include "InfoControl.h"

// CInfoControlService

STDMETHODIMP CInfoControlService::EnableHyperLink(HWND hwndParent)
{
	auto it = m_controls.find(hwndParent);
	if (it == m_controls.end())
	{
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	}
	it->second.m_pControl->EnableHyperLink();
	return S_OK;
}

STDMETHODIMP CInfoControlService::ShowControl(HWND hwndParent, BSTR bstrMessage, BOOL bError, BOOL bMoveChild, HWND* phWnd)
{
	auto it = m_controls.find(hwndParent);
	if (it == m_controls.end())
	{
		ControlData controlData;
		controlData.m_hWndParent = hwndParent;
		controlData.m_hWndChild = GetWindow(hwndParent, GW_CHILD);
		controlData.m_bMoveChild = bMoveChild;

		if (bMoveChild)
		{
			CRect rectChild;
			GetWindowRect(controlData.m_hWndChild, &rectChild);
			CPoint pt(rectChild.left, rectChild.top);
			ScreenToClient(hwndParent, &pt);

			auto height = INFO_CONTROL_HEIGHT;
			pt.y += height;
			rectChild.bottom -= height;
			MoveWindow(controlData.m_hWndChild, pt.x, pt.y, rectChild.right - rectChild.left, rectChild.bottom - rectChild.top, TRUE);
		}

		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_InfoControl, &controlData.m_pControl));

		RETURN_IF_FAILED(controlData.m_pControl->CreateEx(hwndParent, &controlData.m_ControlHwnd));
		CComPtr<IUnknown> pUnk;
		RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
		RETURN_IF_FAILED(AtlAdvise(controlData.m_pControl, pUnk, __uuidof(IInfoControlEventSink), &controlData.m_dwAdvice));
		m_controls[hwndParent] = controlData;
		it = m_controls.find(hwndParent);
		*phWnd = controlData.m_ControlHwnd;
	}

	RETURN_IF_FAILED(it->second.m_pControl->SetMessage(bstrMessage, bError));

	return S_OK;
}

STDMETHODIMP CInfoControlService::HideControl(HWND hwndParent)
{
	auto it = m_controls.find(hwndParent);
	if (it == m_controls.end())
		return S_OK;

	if (it->second.m_bMoveChild)
	{
		CRect rectChild;
		GetWindowRect(it->second.m_hWndChild, &rectChild);
		CPoint pt(rectChild.left, rectChild.top);
		ScreenToClient(it->second.m_hWndParent, &pt);

		auto height = INFO_CONTROL_HEIGHT;
		pt.y -= height;
		rectChild.bottom += height;
		MoveWindow(it->second.m_hWndChild, pt.x, pt.y, rectChild.right - rectChild.left, rectChild.bottom - rectChild.top, TRUE);
	}

	RETURN_IF_FAILED(AtlUnadvise(it->second.m_pControl, __uuidof(IInfoControlEventSink), it->second.m_dwAdvice));
	m_controls.erase(it);
	return S_OK;
}

HRESULT CInfoControlService::Fire_OnLinkClicked(HWND hWnd)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CInfoControlService* pThis = static_cast<CInfoControlService*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		IInfoControlEventSink* pConnection = static_cast<IInfoControlEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnLinkClick(hWnd);
		}
	}
	return hr;
}

STDMETHODIMP CInfoControlService::OnLinkClick(HWND hWnd)
{
	auto it = std::find_if(
		m_controls.begin(),
		m_controls.end(),
		[&](std::pair<HWND, ControlData> pair)
	{
		return pair.second.m_ControlHwnd == hWnd;
	}
	);

	ATLASSERT(it != m_controls.end());

	RETURN_IF_FAILED(Fire_OnLinkClicked(it->second.m_hWndParent));
	return S_OK;
}