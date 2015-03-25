#pragma once

#include "Plugins.h"
#include "..\ObjMdl\\PluginTableBase.h"

using namespace ATL;

#include "ViewMdl_i.h"

class ATL_NO_VTABLE CPluginTable :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPluginTable, &CLSID_PluginTable>,
	public CPluginTableBase
{
public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CPluginTable)
		COM_INTERFACE_ENTRY(IPluginTable)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		RETURN_IF_FAILED(AddObject(CLSID_CommandSupport, _T("Command support object")));
		RETURN_IF_FAILED(AddObject(CLSID_PluginSupport, _T("Plugin support object")));
		RETURN_IF_FAILED(AddObject(CLSID_SimpleListView, _T("Simple list view")));
		RETURN_IF_FAILED(AddObject(CLSID_AboutCommand, _T("About command object")));
		RETURN_IF_FAILED(AddObject(CLSID_MainFrame, _T("Main frame")));
		RETURN_IF_FAILED(AddObject(CLSID_AboutDialog, _T("About dialog")));
		RETURN_IF_FAILED(AddObject(CLSID_CopyCommand, _T("Copy text command")));
		RETURN_IF_FAILED(AddObject(CLSID_SystrayManager, _T("Systray manager")));
		RETURN_IF_FAILED(AddObject(CLSID_HostDialog, _T("Host dialog")));
		RETURN_IF_FAILED(AddObject(CLSID_ActionQueueService, _T("Action queue service")));
		RETURN_IF_FAILED(AddObject(CLSID_InfoControlService, _T("Info control service")));
		RETURN_IF_FAILED(AddObject(CLSID_InfoControl, _T("Info control")));
		RETURN_IF_FAILED(AddObject(CLSID_TabbedControl, _T("Tab control")));
		RETURN_IF_FAILED(AddObject(CLSID_AccelerateDecelerateAnimation, _T("Animation object")));

		RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_CONTAINERWINDOW, CONTROL_TABCONTAINER, CLSID_TabbedControl, _T("Tab control for main frame")));

		RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_COMMAND, COMMAND_FILE, CLSID_FileCommand, _T("File menu item")));
		RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_COMMAND, COMMAND_EXIT, CLSID_ExitCommand, _T("Exit menu item")));

		RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_FORM_MANAGER, CLSID_FormManager, _T("Form manager")));
		RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_BITMAP_SERVICE, CLSID_BitmapService, _T("Bitmap service")));
		RETURN_IF_FAILED(AddPluginInfo(PNAMESP_HOSTFORM, PVIEWTYPE_WINDOW_SERVICE, SERVICE_PROGRESS_DIALOG, CLSID_ProgressDialogService, _T("Progress dialog service")));

		RETURN_IF_FAILED(AddPluginInfo(CONTROL_TABCONTAINER, PVIEWTYPE_COMMAND, COMMAND_TAB_CLOSE, CLSID_TabCloseCommand, _T("Close tab command")));
		
		return S_OK;
	}

	void FinalRelease()
	{
	}

	CPluginTable()
	{
	}
};

OBJECT_ENTRY_AUTO(__uuidof(PluginTable), CPluginTable)
