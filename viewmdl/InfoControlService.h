// InfoControlService.h : Declaration of the CInfoControlService

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"
#include "IInitializeWithControlImpl.h"

using namespace ATL;

// CInfoControlService

class ATL_NO_VTABLE CInfoControlService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CInfoControlService, &CLSID_InfoControlService>,
	public IInfoControlService,
	public IInitializeWithControlImpl,
	public IInfoControlEventSink,
	public IConnectionPointContainerImpl<CInfoControlService>,
	public IConnectionPointImpl<CInfoControlService, &__uuidof(IInfoControlEventSink)>
{
public:
	CInfoControlService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_INFOCONTROLSERVICE)

	BEGIN_COM_MAP(CInfoControlService)
		COM_INTERFACE_ENTRY(IInfoControlService)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IInfoControlEventSink)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CInfoControlService)
		CONNECTION_POINT_ENTRY(__uuidof(IInfoControlEventSink))
	END_CONNECTION_POINT_MAP()

private:
	struct ControlData
	{
		HWND m_hWndParent = 0;
		HWND m_hWndChild = 0;
		HWND m_ControlHwnd = 0;
		CComPtr<IInfoControl> m_pControl;
		BOOL m_bMoveChild = FALSE;
		DWORD m_dwAdvice = 0;
	};

	std::map<HWND, ControlData> m_controls;

	HRESULT Fire_OnLinkClicked(HWND hWnd);

public:

	STDMETHOD(ShowControl)(HWND hwndParent, BSTR bstrMessage, BOOL bError, BOOL bMoveChild);
	STDMETHOD(HideControl)(HWND hwndParent);
	STDMETHOD(EnableHyperLink)(HWND hwndParent);

	STDMETHOD(OnLinkClick)(HWND hWnd);
};

OBJECT_ENTRY_AUTO(__uuidof(InfoControlService), CInfoControlService)
