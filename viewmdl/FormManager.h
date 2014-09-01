// FormManager.h : Declaration of the CFormManager

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"
#include "..\ObjMdl\GUIDComparer.h"

using namespace ATL;


// CFormManager

class ATL_NO_VTABLE CFormManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFormManager, &CLSID_FormManager>,
	public IFormManager,
	public IInitializeWithControlImpl,
	public ITabbedControlEventSink
{
public:
	CFormManager() : m_dwAdvice(0)
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CFormManager)
		COM_INTERFACE_ENTRY(IFormManager)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ITabbedControlEventSink)
	END_COM_MAP()

private:
	std::map< GUID, CAdapt< CComPtr<IControl> >, GUIDComparer > m_pControls;
	DWORD m_dwAdvice;

	STDMETHOD(GetContainerControl)(IContainerControl** ppContainerControl);
public:

	STDMETHOD(OpenForm)(GUID guidId, IControl** ppControl);
	STDMETHOD(FindForm)(GUID guidId, IControl** ppControl);
	STDMETHOD(ActivateForm)(GUID guidId);
	STDMETHOD(OnClose)(IControl* pControl);
	STDMETHOD(CloseAll)();
};

OBJECT_ENTRY_AUTO(__uuidof(FormManager), CFormManager)
