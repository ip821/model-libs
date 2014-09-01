// ExitCommand.h : Declaration of the CExitCommand

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;


// CTabCloseCommand

class ATL_NO_VTABLE CTabCloseCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTabCloseCommand, &CLSID_TabCloseCommand>,
	public ICommand,
	public IInitializeWithControlImpl
{
public:
	CTabCloseCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTabCloseCommand)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ICommand)
	END_COM_MAP()

public:
	
	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CHECK_E_POINTER(bstrText);
		*bstrText = CComBSTR(L"Close").Copy();
		return S_OK;
	}

	STDMETHOD(InstallMenu)(IMenu* pMenu)
	{
		CHECK_E_POINTER(pMenu);
		return pMenu->AddMenuCommand(GUID_NULL, COMMAND_TAB_CLOSE, this);
	}

	STDMETHOD(Invoke)(REFGUID guidCommand)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CComQIPtr<ITabbedControl> pTabbedControl = m_pControl;
		if(!pTabbedControl)
			return E_NOINTERFACE;

		CComPtr<IControl> pControl;
		RETURN_IF_FAILED(pTabbedControl->GetCurrentPage(&pControl));
		RETURN_IF_FAILED(pTabbedControl->RemovePage(pControl));

		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(TabCloseCommand), CTabCloseCommand)
