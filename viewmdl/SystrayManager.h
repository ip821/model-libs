// SystrayManager.h : Declaration of the CSystrayManager

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

#include "ViewMdl_i.h"

using namespace ATL;

// CSystrayManager

class ATL_NO_VTABLE CSystrayManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSystrayManager, &CLSID_SystrayManager>,
	public ISystrayManager
{
public:
	CSystrayManager()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSystrayManager)
		COM_INTERFACE_ENTRY(ISystrayManager)
	END_COM_MAP()

public:
	STDMETHOD(CreateSystrayWindow)(HWND hwndParent, ISystrayWindow** ppWindow);
};

OBJECT_ENTRY_AUTO(__uuidof(SystrayManager), CSystrayManager)
