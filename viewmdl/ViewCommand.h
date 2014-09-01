// ViewCommand.h : Declaration of the CViewCommand

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;


// CViewCommand

class ATL_NO_VTABLE CViewCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CViewCommand, &CLSID_ViewCommand>,
	public ICommand
{
public:
	CViewCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CViewCommand)
		COM_INTERFACE_ENTRY(ICommand)
	END_COM_MAP()

public:
	
	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CHECK_E_POINTER(bstrText);
		*bstrText = CComBSTR(L"View").Copy();
		return S_OK;
	}

	STDMETHOD(InstallMenu)(IMenu* pMenu)
	{
		CHECK_E_POINTER(pMenu);
		return pMenu->InsertMenuCommand(GUID_NULL, COMMAND_VIEW, 1, this);
	}

	STDMETHOD(Invoke)(REFGUID guidCommand)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(ViewCommand), CViewCommand)
