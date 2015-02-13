// FormManager.h : Declaration of the CFormManager

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"
#include "..\ObjMdl\GUIDComparer.h"
#include "..\ObjMdl\Macros.h"

using namespace ATL;

class CFormManager;
typedef IConnectionPointImpl <CFormManager, &__uuidof(IFormManagerEventSink)> IConnectionPointImpl_IFormManagerEventSink;

// CFormManager

class ATL_NO_VTABLE CFormManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFormManager, &CLSID_FormManager>,
	public IFormManager,
	public IInitializeWithControlImpl,
	public IConnectionPointContainerImpl<CFormManager>,
	public IConnectionPointImpl_IFormManagerEventSink,
	public ITabbedControlEventSink,
	public IPluginSupportNotifications
{
public:
	CFormManager()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CFormManager)
		COM_INTERFACE_ENTRY(IFormManager)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ITabbedControlEventSink)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CFormManager)
		CONNECTION_POINT_ENTRY(__uuidof(IFormManagerEventSink))
	END_CONNECTION_POINT_MAP()

private:
	CComPtr<ITabbedControl> m_pTabbedControl;

	std::map< GUID, CComPtr<IControl>, GUIDComparer > m_pControls;
	DWORD m_dwAdvice = 0;

	STDMETHOD(GetContainerControl)(IContainerControl** ppContainerControl);
	HRESULT Fire_OnActivate(IControl* pControl);
	HRESULT Fire_OnDeactivate(IControl* pControl);
	STDMETHOD(OpenFormInternal)(IControl* pControl);

public:

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OpenForm)(GUID guidId, IControl** ppControl);
	STDMETHOD(OpenForm2)(GUID guidNamespace, GUID guidType, GUID guidId, IControl** ppControl);
	STDMETHOD(FindForm)(GUID guidId, IControl** ppControl);
	STDMETHOD(ActivateForm)(GUID guidId);
	STDMETHOD(OnClose)(IControl* pControl);
	STDMETHOD(OnActivate)(IControl* pControl);
	STDMETHOD(OnDeactivate)(IControl* pControl);
	STDMETHOD(OnTabHeaderClick)(IControl* pControl);
	STDMETHOD(CloseForm)(IControl* pControl);
	STDMETHOD(ActivateForm2)(IControl* pControl);
	STDMETHOD(CloseAll)();
	STDMETHOD(GetForms)(IObjArray** ppFormsArray);
};

OBJECT_ENTRY_AUTO(__uuidof(FormManager), CFormManager)
