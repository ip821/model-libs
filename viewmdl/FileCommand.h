// ExitCommand.h : Declaration of the CExitCommand

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;


// CFileCommand

class ATL_NO_VTABLE CFileCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFileCommand, &CLSID_FileCommand>,
	public ICommand
{
public:
	CFileCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CFileCommand)
		COM_INTERFACE_ENTRY(ICommand)
	END_COM_MAP()

public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CHECK_E_POINTER(bstrText);
		*bstrText = CComBSTR(L"File").Copy();
		return S_OK;
	}

	STDMETHOD(InstallMenu)(IMenu* pMenu)
	{
		CHECK_E_POINTER(pMenu);
		return pMenu->AddMenuCommand(GUID_NULL, COMMAND_FILE, this);
	}

	STDMETHOD(Invoke)(REFGUID guidCommand)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(FileCommand), CFileCommand)
