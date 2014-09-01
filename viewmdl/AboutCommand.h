// AboutCommand.h : Declaration of the CExitCommand

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"
#include "AboutDialog.h"

using namespace ATL;

// CAboutCommand

class ATL_NO_VTABLE CAboutCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAboutCommand, &CLSID_ExitCommand>,
	public IInitializeWithControlImpl,
	public ICommand
{
public:
	CAboutCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CAboutCommand)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ICommand)
	END_COM_MAP()

public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CHECK_E_POINTER(bstrText);
		*bstrText = CComBSTR(L"About...").Copy();
		return S_OK;
	}

	STDMETHOD(InstallMenu)(IMenu* pMenu)
	{
		CHECK_E_POINTER(pMenu);
		return pMenu->AddMenuCommand(COMMAND_HELP, COMMAND_ABOUT, this);
	}

	STDMETHOD(Invoke)(REFGUID guidCommand)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CComPtr<IPluginManager> pPluginManager;
		RETURN_IF_FAILED(HrGetPluginManager(&pPluginManager));
		CComPtr<IDialog> pDialog;
		RETURN_IF_FAILED(pPluginManager->CoCreateInstance(CLSID_AboutDialog, IID_IDialog, (LPVOID*)&pDialog));
		INT_PTR res = 0;
		RETURN_IF_FAILED(pDialog->DoModal(::GetActiveWindow(), &res));
		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(AboutCommand), CAboutCommand)
