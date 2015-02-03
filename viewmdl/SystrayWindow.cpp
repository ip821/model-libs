// SystrayWindow.cpp : Implementation of CSystrayWindow

#include "stdafx.h"
#include "SystrayWindow.h"
#include "ViewMdl_i.h"

// CSystrayWindow

CSystrayWindow::CSystrayWindow()
{
}

HRESULT CSystrayWindow::FinalConstruct()
{
	return S_OK;
}

void CSystrayWindow::FinalRelease()
{
}

STDMETHODIMP CSystrayWindow::Hide()
{
	m_trayNotifyIcon.Hide();
	return S_OK;
}

STDMETHODIMP CSystrayWindow::SetTooltip(BSTR bstrToolTip)
{
	m_trayNotifyIcon.SetTooltipText(bstrToolTip);
	return S_OK;
}

STDMETHODIMP CSystrayWindow::Create(HWND hWndParent)
{
	m_trayNotifyIcon.Create(hWndParent, WM_TRAYNOTIFY);
	return S_OK;
}

STDMETHODIMP CSystrayWindow::SetIcon(HICON hIcon)
{
	m_trayNotifyIcon.SetIcon(hIcon);
	return S_OK;
}

STDMETHODIMP CSystrayWindow::SetMenu(HMENU hMenu)
{
	m_trayNotifyIcon.SetMenu(hMenu);
	return S_OK;
}

STDMETHODIMP CSystrayWindow::OnTrayNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lpResult, BOOL* pbHandled)
{
	CHECK_E_POINTER(pbHandled);
	*lpResult = (BOOL)m_trayNotifyIcon.OnTrayNotification(wParam, lParam);
	RETURN_IF_FAILED(Fire_OnSystrayNotification(uMsg, wParam, lParam, *lpResult, *pbHandled));
	return S_OK;
}

STDMETHODIMP CSystrayWindow::ShowBaloon(BSTR bstrText, BSTR bstrCaption, SystrayBalloonStyle style, UINT uTimeout)
{
	m_trayNotifyIcon.SetBalloonDetails(bstrText, bstrCaption, (CTrayNotifyIcon::BalloonStyle)style, uTimeout);
	return S_OK;
}

HRESULT CSystrayWindow::Fire_OnSystrayNotification(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lpResult, BOOL pbHandled)
{
	HRESULT hr = S_OK;
	CSystrayWindow* pThis = static_cast<CSystrayWindow*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		ISystrayWindowEventSink * pConnection = static_cast<ISystrayWindowEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnTrayNotification(uMsg, wParam, lParam, lpResult, pbHandled);
		}
	}
	return hr;
}