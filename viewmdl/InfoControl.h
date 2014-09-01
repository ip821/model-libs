// InfoControl.h : Declaration of the CInfoControl

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;

// CInfoControl

#define INFO_CONTROL_HEIGHT 22

class ATL_NO_VTABLE CInfoControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CInfoControl, &CLSID_InfoControl>,
	public IInfoControl,
	public CAxDialogImpl<CInfoControl>,
	public CDialogResize<CInfoControl>,
	public IInitializeWithControlImpl,
	public IConnectionPointContainerImpl<CInfoControl>,
	public IConnectionPointImpl<CInfoControl, &__uuidof(IInfoControlEventSink)>
{
public:
	enum { IDD = IDD_INFOCONTROL };

	CInfoControl()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_INFOCONTROL)

	BEGIN_COM_MAP(CInfoControl)
		COM_INTERFACE_ENTRY(IInfoControl)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CInfoControl)
		CONNECTION_POINT_ENTRY(__uuidof(IInfoControlEventSink))
	END_CONNECTION_POINT_MAP()

	BEGIN_DLGRESIZE_MAP(CInfoControl)
		DLGRESIZE_CONTROL(IDC_LABEL, DLSZ_SIZE_X)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CInfoControl)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		NOTIFY_HANDLER(IDC_LABEL, NM_CLICK, OnLinkClick)
		CHAIN_MSG_MAP(CDialogResize<CInfoControl>)
		CHAIN_MSG_MAP(CAxDialogImpl<CInfoControl>)
	END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct();
	void FinalRelease();

private:
	CStatic m_pictureBox;
	CStatic m_label;
	CHyperLink m_hyperLink;
	CIcon m_iconInfo;
	CIcon m_iconError;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLinkClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	HRESULT Fire_OnLinkClicked(HWND hWnd);

public:

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(SetMessage)(BSTR bstrMessage, BOOL bError);
	STDMETHOD(EnableHyperLink)();
};

OBJECT_ENTRY_AUTO(__uuidof(InfoControl), CInfoControl)
