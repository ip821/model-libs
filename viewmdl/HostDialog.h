// HostDialog.h : Declaration of the CHostDialog

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"
#include "ChildDialogControl.h"

using namespace ATL;

// CHostDialog

class ATL_NO_VTABLE CHostDialog :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHostDialog, &CLSID_HostDialog>,
	public CAxDialogImpl<CHostDialog>,
	public CDialogResize<CHostDialog>,
	public CChildDialogControlSupport<CHostDialog>,
	public IDialog,
	public IInitializeWithControlImpl,
	public IPersistSettings
{
public:
	CHostDialog()
	{
	}

	enum { IDD = IDD_HOSTDIALOG };

	DECLARE_NO_REGISTRY()
	
	BEGIN_COM_MAP(CHostDialog)
		COM_INTERFACE_ENTRY(IDialog)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPersistSettings)
	END_COM_MAP()

	BEGIN_MSG_MAP(CHostDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
		CHAIN_MSG_MAP(CDialogResize<CHostDialog>)
		CHAIN_MSG_MAP(CAxDialogImpl<CHostDialog>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CHostDialog)
		DLGRESIZE_CONTROL(IDC_PLACEHOLDER2, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

private:
	CComPtr<ISettings> m_pSettingsToLoad;

	void UpdateCaption();
	void UpdateSize();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

public:
	STDMETHOD(DoModal)(HWND hWnd, INT_PTR* pResult);

	STDMETHOD(Load)(ISettings *pSettings);
	STDMETHOD(Save)(ISettings *pSettings);
	STDMETHOD(Reset)(ISettings *pSettings);
};

OBJECT_ENTRY_AUTO(__uuidof(HostDialog), CHostDialog)
