// SystrayWindow.h : Declaration of the CSystrayWindow

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;


// CSystrayWindow

class ATL_NO_VTABLE CSystrayWindow :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSystrayWindow, &CLSID_SystrayWindow>,
	public ISystrayWindow,
	public IConnectionPointContainerImpl<CSystrayWindow>,
	public IConnectionPointImpl<CSystrayWindow, &__uuidof(ISystrayWindowEventSink)>
{
public:
	CSystrayWindow();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSystrayWindow)
		COM_INTERFACE_ENTRY(ISystrayWindow)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CSystrayWindow)
		CONNECTION_POINT_ENTRY(__uuidof(ISystrayWindowEventSink))
	END_CONNECTION_POINT_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT();

	HRESULT FinalConstruct();
	void FinalRelease();

public:
	STDMETHOD(Create)(HWND hWndParent);
	STDMETHOD(SetIcon)(HICON hIcon);
	STDMETHOD(SetMenu)(HMENU hMenu);
	STDMETHOD(OnTrayNotify)(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lpResult, BOOL* pbHandled);
	STDMETHOD(ShowBaloon)(BSTR bstrText, BSTR bstrCaption, SystrayBalloonStyle style, UINT uTimeout);
	STDMETHOD(Hide)();

	HRESULT Fire_OnSystrayNotification(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT lpResult, BOOL pbHandled);
private:
	CTrayNotifyIcon m_trayNotifyIcon;
};

OBJECT_ENTRY_AUTO(__uuidof(SystrayWindow), CSystrayWindow)
