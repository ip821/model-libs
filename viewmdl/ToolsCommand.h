// ToolsCommand.h : Declaration of the CToolsCommand

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;


// CToolsCommand

class ATL_NO_VTABLE CToolsCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CToolsCommand, &CLSID_ToolsCommand>,
	public ICommand
{
public:
	CToolsCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CToolsCommand)
		COM_INTERFACE_ENTRY(ICommand)
	END_COM_MAP()

public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CHECK_E_POINTER(bstrText);
		*bstrText = CComBSTR(L"Tools").Copy();
		return S_OK;
	}

	STDMETHOD(InstallMenu)(IMenu* pMenu)
	{
		CHECK_E_POINTER(pMenu);
		return pMenu->InsertMenuCommand(GUID_NULL, COMMAND_TOOLS, 2, this);
	}

	STDMETHOD(Invoke)(REFGUID guidCommand)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(ToolsCommand), CToolsCommand)
