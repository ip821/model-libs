// StatusBarSupport.cpp : Implementation of CCStatusBarSupport

#include "stdafx.h"
#include "StatusBarSupport.h"


// CStatusBarSupport

STDMETHODIMP CStatusBarSupport::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
	m_statusBar.Attach(m_hWnd);
	return S_OK;
}

STDMETHODIMP CStatusBarSupport::GetHWND(HWND* hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CStatusBarSupport::PreTranslateMessage(MSG* pMsg, BOOL* pbResult)
{
	UNREFERENCED_PARAMETER(pMsg);
	UNREFERENCED_PARAMETER(pbResult);
	return S_OK;
}

STDMETHODIMP CStatusBarSupport::CreateEx(HWND hWndParent, HWND* phWnd)
{
	UNREFERENCED_PARAMETER(hWndParent);
	UNREFERENCED_PARAMETER(phWnd);
	return E_NOTIMPL;
}

STDMETHODIMP CStatusBarSupport::GetPartsCount(int* dwPartsCount)
{
	CHECK_E_POINTER(dwPartsCount);
	*dwPartsCount = m_statusBar.GetParts(0, NULL);
	return S_OK;
}

STDMETHODIMP CStatusBarSupport::GetParts(int nParts, int* pWidths)
{
	CHECK_E_POINTER(pWidths);
	m_statusBar.GetParts((int)nParts, pWidths);
	return S_OK;
}

STDMETHODIMP CStatusBarSupport::SetParts(int nParts, int* pWidths)
{
	m_statusBar.SetParts((int)nParts, pWidths);
	return S_OK;
}

STDMETHODIMP CStatusBarSupport::GetPartRect(int nPart, LPRECT lpRect)
{
	CHECK_E_POINTER(lpRect);
	m_statusBar.GetRect((int)nPart, lpRect);
	return S_OK;
}
