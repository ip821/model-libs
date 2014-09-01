// ExitCommand.h : Declaration of the CExitCommand

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"
#include "IInitializeWithControlImpl.h"

using namespace ATL;

// CExitCommand

class ATL_NO_VTABLE CExitCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CExitCommand, &CLSID_ExitCommand>,
	public IInitializeWithControlImpl,
	public ICommand
{
public:
	CExitCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CExitCommand)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ICommand)
	END_COM_MAP()

public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CHECK_E_POINTER(bstrText);
		*bstrText = CComBSTR(L"Exit").Copy();
		return S_OK;
	}

	STDMETHOD(InstallMenu)(IMenu* pMenu)
	{
		CHECK_E_POINTER(pMenu);
		return pMenu->AddMenuCommand(COMMAND_FILE, COMMAND_EXIT, this);
	}

	STDMETHOD(Invoke)(REFGUID guidCommand)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		HWND hwnd = NULL;
		HRESULT hr = m_pControl->GetHWND(&hwnd);
		if(FAILED(hr))
			return hr;

		::PostMessage(hwnd, WM_CLOSE, 0, 0);
		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(ExitCommand), CExitCommand)
