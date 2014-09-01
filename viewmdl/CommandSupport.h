// CommandSupport.h : Declaration of the CCommandSupport

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"
#include "CProxyICommandSupportEventSink.h"
#include "..\ObjMdl\GUIDComparer.h"

using namespace ATL;

// CCommandSupport

class ATL_NO_VTABLE CCommandSupport :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCommandSupport, &CLSID_CommandSupport>,
	public ICommandSupport,
	public IMenu,
	public IToolbar,
	public IInitializeWithControlImpl,
	public IIdleHandler,
	public IConnectionPointContainerImpl<CCommandSupport>,
	public CProxyICommandSupportEventSink<CCommandSupport>,
	public IInitializeWithVariantObject,
	public IInitializeWithColumnName
{
public:
	CCommandSupport() : m_CmdIdCounter(1000)
	{
		m_imageList.Create(16, 16, ILC_COLOR32, 0, 0);
		m_bInstallStdCommands = TRUE;
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CCommandSupport)
		COM_INTERFACE_ENTRY(ICommandSupport)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IMenu)
		COM_INTERFACE_ENTRY(IToolbar)
		COM_INTERFACE_ENTRY(IIdleHandler)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithColumnName)
	END_COM_MAP()

	BEGIN_MSG_MAP(CCommandSupport)
		COMMAND_CODE_HANDLER(0, OnCommandClick)
	END_MSG_MAP()

	BEGIN_CONNECTION_POINT_MAP(CCommandSupport)
		CONNECTION_POINT_ENTRY(__uuidof(ICommandSupportEventSink))
	END_CONNECTION_POINT_MAP()

private:
	std::map<int, CAdapt<CComPtr<ICommand> > > m_InstalledCommandsMap;
	std::map<GUID, std::list<int>, GUIDComparer> m_InstalledCommandsGuidToIdMap;
	std::map<int, GUID> m_InstalledCommandsIdToGuidMap;
	std::map<GUID, BOOL, GUIDComparer> m_commandState;
	int m_CmdIdCounter;

	LRESULT OnCommandClick(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	STDMETHOD(InsertCommandToMenu)(REFGUID guidParentCommand, REFGUID guidCommand, UINT ulIndex, ICommand* pCommand);
	STDMETHOD(InstallStdParentMenuItemsIfNeed)(REFGUID guidParentCommand);
	STDMETHOD(RegisterCommand)(ICommand* pCommand, REFGUID gCommand, UINT cmdId);

	BOOL m_bInstallStdCommands;
	CString m_strColumnName;
	CImageListManaged m_imageList;
	CMenuHandle m_menu;
	CToolBarCtrl m_toolBar;
	HWND m_hWndToolBar;
	CComPtr<IBitmapService> m_pBitmapService;
	CComPtr<IVariantObject> m_pVariantObject;

private:

	STDMETHOD(EnableCommandInternal)(REFGUID guidCommand, BOOL bEnable);

public:

	STDMETHOD(GetImageList)(HIMAGELIST* phImageList);
	STDMETHOD(InstallCommand)(ICommand* pCommand);
	STDMETHOD(InstallCommands)(IPluginSupport* pPluginSupport);
	STDMETHOD(UninstallAll)();
	STDMETHOD(SetMenu)(HMENU hMenu);
	STDMETHOD(SetToolbar)(HWND hToolbar);
	STDMETHOD(SetBitmapService)(IBitmapService* pBitmapService);

	STDMETHOD(GetHWND)(HWND* hWnd);
	STDMETHOD(AddMenuCommand)(REFGUID guidParentCommand, REFGUID guidCommand, ICommand* pCommand);
	STDMETHOD(InsertMenuCommand)(REFGUID guidParentCommand, REFGUID guidCommand, UINT ulIndex, ICommand* pCommand);

	STDMETHOD(AddToolbarCommand)(REFGUID guidCommand, ICommand2* pCommand);
	STDMETHOD(InsertToolbarCommand)(REFGUID guidCommand, UINT ulIndex, ICommand2* pCommand);

	STDMETHOD(OnIdle)(BOOL* bResult);
	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult);
	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
	STDMETHOD(SetColumnName)(LPCTSTR lpszColumnName);
	STDMETHOD(EnableStdCommands)(BOOL bEnable);
	STDMETHOD(QueueCommandExecution)(GUID guidCommand, VARIANT* vParam);
	STDMETHOD(EnableCommand)(REFGUID guidCommand, BOOL bEnable);
};

OBJECT_ENTRY_AUTO(__uuidof(CommandSupport), CCommandSupport)
