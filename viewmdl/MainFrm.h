// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ViewMdl_i.h"
#include "StatusBarSupport.h"
#include "PluginSupport.h"
#include "CommandSupport.h"

#define WINDOW_MENU_POSITION	3

#define SETTINGS_LEFT L"Left"
#define SETTINGS_TOP L"Top"
#define SETTINGS_BOTTOM L"Bottom"
#define SETTINGS_RIGHT L"Right"
#define SETTINGS_SHOWCMD L"ShowCmd"

class ATL_NO_VTABLE CMainFrame : 
	public CFrameWindowImpl<CMainFrame>, 
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, 
	public CIdleHandler,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMainFrame, &CLSID_MainFrame>,
	public IMainWindow,
	public IMsgFilter,
	public IIdleHandler,
	public IPersistSettings
{
public:
	CMainFrame()
	{
		ZeroMemory((LPVOID)&m_rectForSettingsSave, sizeof(m_rectForSettingsSave));
		ZeroMemory((LPVOID)&m_placement, sizeof(m_placement));
	}

	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CMainFrame)
		COM_INTERFACE_ENTRY(IMainWindow)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IMsgFilter)
		COM_INTERFACE_ENTRY(IPersistSettings)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_ICommandSupport, m_pCommandSupport)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IMenu, m_pCommandSupport)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IToolbar, m_pCommandSupport)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IServiceProvider, m_pPluginSupport)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IStatusBar, m_pStatusBarSupport)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_RANGE_HANDLER(ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
		bHandled = TRUE;
		lResult = OnMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled)
			return TRUE;
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnWindowActivate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void UpdateLayout(BOOL bResizeBars);

private:

	CComPtr<IPluginManager> m_pPluginManager;
	CComPtr<CStatusBarSupport> m_pStatusBarSupport;
	CComPtr<CPluginSupport> m_pPluginSupport;
	CComPtr<CCommandSupport> m_pCommandSupport;
	CComPtr<IMessageLoop> m_pMessageLoop;
	CComPtr<IContainerControl> m_pContainerControl;
	CComPtr<ISettings> m_pSettings;

	CMultiPaneStatusBarCtrl m_statusBar;
	CMenuHandle m_menuMain;
	CToolBarCtrl m_toolBar;
	CIconHandle m_IconHandle;
	RECT m_rectForSettingsSave;
	WINDOWPLACEMENT m_placement;
	UINT m_CmdShow = 0;
	MainWindowFlags m_flags = MainWindowFlags::MainWindowAll;

	STDMETHOD(InitContainerControl)();

public:
	STDMETHOD(SetFlags)(MainWindowFlags flags);
	STDMETHOD(GetMessageLoop)(IMessageLoop** ppMessageLoop);
	STDMETHOD(SetMessageLoop)(IMessageLoop* pMessageLoop);
	STDMETHOD(GetHWND)(HWND* hWnd);
	STDMETHOD(PreTranslateMessage)(MSG* pMsg, BOOL* pbResult);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND* phWnd);
	STDMETHOD(ShowWindow)(int cmd, BOOL* bResult);
	STDMETHOD(GetContainerControl)(IContainerControl** ppContainerControl);
	STDMETHOD(SetContainerControl)(IContainerControl* pContainerControl);
	STDMETHOD(SetIcon)(HICON hIcon, BOOL bLarge);

	STDMETHOD(OnIdle)(BOOL *bResult);
	STDMETHOD(GetMainMenu)(IMenu** ppMenu);
	STDMETHOD(GetToolbar)(IToolbar** ppToolbar);
	STDMETHOD(GetStatusBar)(IStatusBar** ppStatusBar);

	STDMETHOD(Load)(ISettings* pSettings);
	STDMETHOD(Save)(ISettings* pSettings);
	STDMETHOD(Reset)(ISettings* pSettings);
};

OBJECT_ENTRY_AUTO(__uuidof(MainFrame), CMainFrame)