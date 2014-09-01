// HelpCommand.h : Declaration of the CExitCommand

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;

// CHelpCommand

class ATL_NO_VTABLE CHelpCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHelpCommand, &CLSID_ExitCommand>,
	public IInitializeWithControlImpl,
	public ICommand
{
public:
	CHelpCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CHelpCommand)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ICommand)
	END_COM_MAP()

public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CHECK_E_POINTER(bstrText);
		*bstrText = CComBSTR(L"Help").Copy();
		return S_OK;
	}

	STDMETHOD(InstallMenu)(IMenu* pMenu)
	{
		CHECK_E_POINTER(pMenu);
		return pMenu->AddMenuCommand(GUID_NULL, COMMAND_HELP, this);
	}

	STDMETHOD(Invoke)(REFGUID guidCommand)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(HelpCommand), CHelpCommand)
