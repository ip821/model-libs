// InfoControl.cpp : Implementation of CInfoControl

#include "stdafx.h"
#include "InfoControl.h"


// CInfoControl

HRESULT CInfoControl::FinalConstruct()
{
	return S_OK;
}

void CInfoControl::FinalRelease()
{
	if (m_hWnd)
		DestroyWindow();
}

STDMETHODIMP CInfoControl::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CInfoControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	m_hWnd = __super::Create(hWndParent);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CInfoControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(pbResult);
	return S_OK;
}

LRESULT CInfoControl::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_label.Attach(GetDlgItem(IDC_LABEL));

	m_iconInfo.LoadIcon(IDI_ICON_INFO, 16, 16);
	m_iconError.LoadIcon(IDI_ICON_ERROR, 16, 16);

	m_pictureBox.Attach(GetDlgItem(IDC_PICTUREBOX));
	m_pictureBox.SetIcon(m_iconInfo);

	//size adjust
	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(&rect);
	MoveWindow(rect.left, rect.top, rect.right - rect.left, INFO_CONTROL_HEIGHT);
	m_pictureBox.MoveWindow(3, 3, 16, 16);

	CRect labelRect;
	m_label.GetWindowRect(&labelRect);
	labelRect.top -= 6;
	labelRect.left -= 5;
	ScreenToClient(&labelRect);
	m_label.MoveWindow(labelRect);
	return 0;
}

STDMETHODIMP CInfoControl::SetMessage(BSTR bstrMessage, BOOL bError)
{
	m_pictureBox.SetIcon(bError ? m_iconError : m_iconInfo);
	if (m_label.IsWindow())
	{
		m_label.SetWindowText(bstrMessage);
	}
	else if (m_hyperLink.IsWindow())
	{
		m_hyperLink.SetWindowText(bstrMessage);
	}
	BringWindowToTop();
	return S_OK;
}

STDMETHODIMP CInfoControl::EnableHyperLink()
{
	auto hwnd = m_label.Detach();
	if (!m_hyperLink.IsWindow())
	{
		m_hyperLink.SubclassWindow(hwnd);
		m_hyperLink.SetHyperLinkExtendedStyle(m_hyperLink.GetHyperLinkExtendedStyle() | HLINK_NOTIFYBUTTON);
	}
	return S_OK;
}

HRESULT CInfoControl::Fire_OnLinkClicked(HWND hWnd)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CInfoControl* pThis = static_cast<CInfoControl*>(this);
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

LRESULT CInfoControl::OnLinkClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	Fire_OnLinkClicked(m_hWnd);
	return 0;
}