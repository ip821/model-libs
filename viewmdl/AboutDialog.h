// AboutDialog.h : Declaration of the CAboutDialog

#pragma once

#include "resource.h"       // main symbols
#include <atlhost.h>
#include "ChildDialogControl.h"
using namespace ATL;

// CAboutDialog

class ATL_NO_VTABLE CAboutDialog : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAboutDialog, &CLSID_AboutDialog>,
	public CAxDialogImpl<CAboutDialog>,
	public CChildDialogControlSupport<CAboutDialog>,
	public IDialog
{
public:

	CAboutDialog()
	{
	}

	enum { IDD = IDD_ABOUTDIALOG };

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CAboutDialog)
		COM_INTERFACE_ENTRY(IDialog)
	END_COM_MAP()

	BEGIN_MSG_MAP(CAboutDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
		COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
		CHAIN_MSG_MAP(CAxDialogImpl<CAboutDialog>)
	END_MSG_MAP()

	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

private:
	CComPtr<IPluginManager> m_pPluginManager;
	CComPtr<ISimpleListView> m_pSimpleListView;
	HWND m_hWnd2;
	CFont m_font;

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	STDMETHOD(InitListView)();

public:
	STDMETHOD(DoModal)(HWND hWnd, INT_PTR* pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(AboutDialog), CAboutDialog)