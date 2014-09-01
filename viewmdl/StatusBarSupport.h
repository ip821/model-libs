// CStatusBarSupport.h : Declaration of the CCStatusBarSupport

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;


// CCStatusBarSupport

class ATL_NO_VTABLE CStatusBarSupport :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CStatusBarSupport, &CLSID_StatusBarSupport>,
	public IStatusBar
{
public:
	CStatusBarSupport()
	{
		m_hWnd = 0;
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CStatusBarSupport)
		COM_INTERFACE_ENTRY(IStatusBar)
	END_COM_MAP()

private:
	HWND m_hWnd;
	CStatusBarCtrl m_statusBar;
public:
	STDMETHOD(SetHWND)(HWND hWnd);

	STDMETHOD(GetHWND)(HWND* hWnd);
	STDMETHOD(PreTranslateMessage)(MSG* pMsg, BOOL* pbResult);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND* phWnd);

	STDMETHOD(GetPartsCount)(int* pdwPartsCount);
	STDMETHOD(GetParts)(int nParts, int* pWidths);
	STDMETHOD(SetParts)(int nParts, int* pWidths);
	STDMETHOD(GetPartRect)(int nPart, LPRECT lpRect);
};

OBJECT_ENTRY_AUTO(__uuidof(StatusBarSupport), CStatusBarSupport)
