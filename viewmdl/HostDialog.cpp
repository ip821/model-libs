// HostDialog.cpp : Implementation of CHostDialog

#include "stdafx.h"
#include "HostDialog.h"


// CHostDialog
LRESULT CHostDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DlgResize_Init(false);
	CAxDialogImpl<CHostDialog>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	RETURN_IF_FAILED(InsertToGroupboxPlaceholder(m_pControl, IDC_PLACEHOLDER2, FALSE));

	CComQIPtr<IPersistSettings> pPersistSettings = m_pControl;
	if(pPersistSettings)
	{
		pPersistSettings->Load(m_pSettingsToLoad);
	}

	UpdateCaption();
	UpdateSize();
	ATLENSURE(CenterWindow(GetParent()));
	bHandled = TRUE;
	return 1;  // Let the system set the focus
}

void CHostDialog::UpdateCaption()
{
	HWND hWnd = 0;
	if(m_pControl->GetHWND(&hWnd) != S_OK)
		return;
	CString strCaption;
	ATLENSURE(CWindow(hWnd).GetWindowText(strCaption));
	if(!strCaption.IsEmpty())
	{
		ATLENSURE(SetWindowText(strCaption));
	}
}

void CHostDialog::UpdateSize()
{
	HWND hWnd = 0;
	if(m_pControl->GetHWND(&hWnd) != S_OK)
		return;

	RECT rectChildControl = {0};
	CWindow wndChildControl = hWnd;
	ATLENSURE(wndChildControl.GetWindowRect(&rectChildControl));
	ATLENSURE(AdjustWindowRect(&rectChildControl, WS_CAPTION, FALSE));
	rectChildControl.bottom += 20; //for buttons
	ATLENSURE(SetWindowPos(NULL, &rectChildControl, 0));
	RECT rectPlaceHolder = {0};
	ATLENSURE(GetDlgItem(IDC_PLACEHOLDER2).GetWindowRect(&rectPlaceHolder));
	ATLENSURE(ScreenToClient(&rectPlaceHolder));
	ATLENSURE(wndChildControl.MoveWindow(&rectPlaceHolder));
}

STDMETHODIMP CHostDialog::DoModal(HWND hWnd, INT_PTR* pResult)
{
	CHECK_E_POINTER(pResult);
	*pResult = __super::DoModal(hWnd);
	return S_OK;
}

STDMETHODIMP CHostDialog::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettingsToLoad = pSettings;
	return S_OK;
}

STDMETHODIMP CHostDialog::Save(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	CComQIPtr<IPersistSettings> pPersistSettings = m_pControl;
	if(!pPersistSettings)
		return S_OK;

	return pPersistSettings->Save(pSettings);
}

STDMETHODIMP CHostDialog::Reset(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	CComQIPtr<IPersistSettings> pPersistSettings = m_pControl;
	if(!pPersistSettings)
		return S_OK;

	return pPersistSettings->Reset(pSettings);
}

LRESULT CHostDialog::OnClickedOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Save(m_pSettingsToLoad);
	EndDialog(wID);
	return 0;
}

LRESULT CHostDialog::OnClickedCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
