#include "stdafx.h"
#include "ProgressDialogService.h"

HRESULT CProgressDialogService::FinalConstruct()
{
	return S_OK;
}

void CProgressDialogService::FinalRelease()
{

}

STDMETHODIMP CProgressDialogService::StartProgress(BOOL bMarquee)
{
	::PostMessage(m_hControlWnd, WM_PROGRESS_START, bMarquee, 0);
	return S_OK;
}

STDMETHODIMP CProgressDialogService::SetProgress(ULONG ulCompleted, ULONG ulTotal)
{
	::PostMessage(m_hControlWnd, WM_PROGRESS_SETPROGRESS, ulCompleted, ulTotal);
	return S_OK;
}

STDMETHODIMP CProgressDialogService::EndProgress()
{
	::PostMessage(m_hControlWnd, WM_PROGRESS_END, 0, 0);
	return S_OK;
}

STDMETHODIMP CProgressDialogService::SetProgressText(BSTR bstrText)
{
	m_strText = bstrText;
	::PostMessage(m_hControlWnd, WM_PROGRESS_SETTEXT, 0, 0);
	return S_OK;
}

STDMETHODIMP CProgressDialogService::SetProgressCaption(BSTR bstrText)
{
	m_strTextCaption = bstrText;
	::PostMessage(m_hControlWnd, WM_PROGRESS_SETCAPTION, 0, 0);
	return S_OK;
}

STDMETHODIMP CProgressDialogService::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(uMsg);
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	UNREFERENCED_PARAMETER(plResult);
	UNREFERENCED_PARAMETER(bResult);

	switch (uMsg)
	{
	case WM_PROGRESS_START:
	{
							  RETURN_IF_FAILED(CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (LPVOID*)&m_pProgressDialog));
							  auto flags = PROGDLG_NOTIME | PROGDLG_MODAL;
#ifndef __WINXP__
							  flags |= PROGDLG_NOCANCEL;
							  if (wParam == TRUE)
								  flags |= PROGDLG_MARQUEEPROGRESS;
#endif
							  RETURN_IF_FAILED(m_pProgressDialog->StartProgressDialog(m_hControlWnd, NULL, flags, NULL));
							  break;
	}
	case WM_PROGRESS_END:
	{
							RETURN_IF_FAILED(m_pProgressDialog->StopProgressDialog());
							m_pProgressDialog.Release();
							break;
	}
	case WM_PROGRESS_SETTEXT:
	{
								RETURN_IF_FAILED(m_pProgressDialog->SetLine(0, m_strText, FALSE, NULL));
								break;
	}
	case WM_PROGRESS_SETCAPTION:
	{
								   RETURN_IF_FAILED(m_pProgressDialog->SetTitle(m_strTextCaption));
								   break;
	}
	case WM_PROGRESS_SETPROGRESS:
	{
								RETURN_IF_FAILED(m_pProgressDialog->SetProgress(static_cast<DWORD>(wParam), static_cast<DWORD>(lParam)));
									break;
	}
	}
	return S_OK;
}