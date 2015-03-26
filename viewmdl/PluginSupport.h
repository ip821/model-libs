// PluginSupport.h : Declaration of the CPluginSupport

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"
#include "..\ObjMdl\GUIDComparer.h"

using namespace ATL;

// CPluginSupport

class ATL_NO_VTABLE CPluginSupport :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPluginSupport, &CLSID_PluginSupport>,
	public IPluginSupport,
	public IServiceProvider,
	public IMsgHandler,
	public IInitializeWithControl,
	public IInitializeWithSettings,
	public IInitializeWithVariantObject
{
public:
	CPluginSupport()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CPluginSupport)
		COM_INTERFACE_ENTRY(IPluginSupport)
		COM_INTERFACE_ENTRY(IServiceProvider)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IMsgHandler)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease(){}

private:
	typedef std::map<GUID, CComPtr<IUnknown>, GUIDComparer > TPluginMap;
	TPluginMap m_PluginsMap;
	std::vector<CComPtr<IUnknown>> m_Plugins;
	CComPtr<IControl> m_pControl;
	CComPtr<IPluginManager> m_pPluginManager;
	CComPtr<ISettings> m_pSettings;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IVariantObject> m_pVariantObject;
	BOOL bb = FALSE;


public:

	STDMETHOD(SetParentServiceProvider)(IServiceProvider* pServiceProvider);
	STDMETHOD(InitializePlugins)(REFGUID guidNamespace, REFGUID guidType);
	STDMETHOD(GetPlugins)(IObjArray** ppObjectArray);
	STDMETHOD(SetControl)(IControl* pControl);
	STDMETHOD(OnInitialized)();
	STDMETHOD(OnShutdown)();
	STDMETHOD(OnInitializing)();
	STDMETHOD(OnInitCompleted)();

	STDMETHOD(QueryService)(REFGUID guidService, REFIID iid, void** ppServiceObject);
	STDMETHOD(Load)(ISettings* pSettings);
	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);

	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult);
};

OBJECT_ENTRY_AUTO(__uuidof(PluginSupport), CPluginSupport)
