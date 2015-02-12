// TabbedControl.h : Declaration of the CTabbedControl

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"
#include "ITabbedControlEventSink_CP.h"
#include "PluginSupport.h"
#include "CommandSupport.h"

using namespace ATL;

// CTabbedControl

class ATL_NO_VTABLE CTabbedControl :
	public CTabViewImpl<CTabbedControl>,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTabbedControl, &CLSID_TabbedControl>,
	public ITabbedControl,
	public IPersistSettings,
	public IControl,
	public IInitializeWithControlImpl,
	public IConnectionPointContainerImpl<CTabbedControl>,
	public CProxyITabbedControlEventSink<CTabbedControl>,
	public IPluginSupportNotifications
{
public:
	CTabbedControl()
	{
		m_popupMenu.CreatePopupMenu();
	}

	DECLARE_WND_CLASS_EX(_T("WTL_TabView"), 0, COLOR_APPWORKSPACE)

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTabbedControl)
		COM_INTERFACE_ENTRY(ITabbedControl)
		COM_INTERFACE_ENTRY(IContainerControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IPersistSettings)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY_AGGREGATE(__uuidof(IServiceProvider), m_pControl)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_ICommandSupport, m_pCommandSupport)
	END_COM_MAP()

	BEGIN_MSG_MAP(CTabbedControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		CHAIN_MSG_MAP_MEMBER((*m_pCommandSupport))
		CHAIN_MSG_MAP(CTabViewImpl<CTabbedControl>)
		ALT_MSG_MAP(1)
		MESSAGE_HANDLER(WM_MBUTTONUP, OnTabMButtonUp)
		CHAIN_MSG_MAP_ALT(CTabViewImpl<CTabbedControl>, 1)
	END_MSG_MAP()

	BEGIN_CONNECTION_POINT_MAP(CTabbedControl)
		CONNECTION_POINT_ENTRY(__uuidof(ITabbedControlEventSink))
	END_CONNECTION_POINT_MAP()

private:
	std::map<HWND, CComPtr<IControl>> m_pControls;
	CMenu m_popupMenu;
	CComPtr<IPluginManager> m_pPluginManager;
	CComPtr<CPluginSupport> m_PluginSupport;
	CComPtr<CCommandSupport> m_pCommandSupport;
	CComPtr<ISettings> m_pSettings;
	CComPtr<IServiceProvider> m_pServiceProvider;
	BOOL m_bCommandsEnabled = TRUE;

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTabMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);

public:
	HRESULT FinalConstruct();

	void OnPageActivated(int nPage);
	void OnContextMenu(int nPage, POINT pt);

	STDMETHOD(GetHWND)(HWND* hWnd);
	STDMETHOD(PreTranslateMessage)(MSG* pMsg, BOOL* pbResult);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND* phWnd);

	STDMETHOD(EnableCommands)(BOOL bEnabled);
	STDMETHOD(AddPage)(IControl* pControl);
	STDMETHOD(GetCurrentPage)(IControl** ppControl);
	STDMETHOD(RemovePage)(IControl* pControl);
	STDMETHOD(UpdateControl)(IControl* pControl);
	STDMETHOD(ActivatePage)(IControl* pControl);
	STDMETHOD(GetPageCount)(DWORD* pdwCount);
	STDMETHOD(GetPage)(DWORD dwIndex, IControl** ppControl);

	STDMETHOD(Load)(ISettings* pSettings);
	STDMETHOD(Save)(ISettings* pSettings);
	METHOD_EMPTY(STDMETHOD(Reset)(ISettings* /*pSettings*/));

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();
};

OBJECT_ENTRY_AUTO(__uuidof(TabbedControl), CTabbedControl)
