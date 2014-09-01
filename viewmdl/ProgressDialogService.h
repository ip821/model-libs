// ProgressDialogService.h : Declaration of the CProgressDialogService

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;

// CProgressDialogService

class ATL_NO_VTABLE CProgressDialogService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CProgressDialogService, &CLSID_ProgressDialogService>,
	public IProgressService,
	public IMsgHandler,
	public IInitializeWithControlImpl
{
public:
	CProgressDialogService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CProgressDialogService)
		COM_INTERFACE_ENTRY(IProgressService)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IMsgHandler)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct();
	void FinalRelease();

private:
	CComPtr<IProgressDialog> m_pProgressDialog;
	CString m_strText;
	CString m_strTextCaption;
public:
	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult);
	STDMETHOD(StartProgress)(BOOL bMarquee = TRUE);
	STDMETHOD(SetProgressCaption)(BSTR bstrText);
	STDMETHOD(SetProgressText)(BSTR bstrText);
	STDMETHOD(SetProgress)(ULONG ulCompleted, ULONG ulTotal);
	STDMETHOD(EndProgress)();
};

OBJECT_ENTRY_AUTO(__uuidof(ProgressDialogService), CProgressDialogService)
