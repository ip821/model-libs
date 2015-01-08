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
	public IInitializeWithControl,
	public IInitializeWithVariantObject,
	public IInitializeWithSettings
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
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease(){}

private:
	typedef std::map<GUID, CAdapt<CComPtr<IUnknown> >, GUIDComparer > TPluginMap;
	TPluginMap m_PluginsMap;
	std::vector< CAdapt<CComPtr<IUnknown> > > m_Plugins;
	CComPtr<IControl> m_pControl;
	CComPtr<IPluginManager> m_pPluginManager;
	CComPtr<ISettings> m_pSettings;
	CComPtr<IServiceProvider> m_pServiceProvider;

	STDMETHOD(InitWithControl)(IUnknown* pUnk);
	STDMETHOD(InitWithSettings)(IUnknown* pUnk);

public:

	STDMETHOD(SetParentServiceProvider)(IServiceProvider* pServiceProvider);
	STDMETHOD(InitializePlugins)(REFGUID guidNamespace, REFGUID guidType);
	STDMETHOD(GetPlugins)(IObjArray** ppObjectArray);
	STDMETHOD(SetControl)(IControl* pControl);
	STDMETHOD(UninstallAll)();
	STDMETHOD(OnInitialized)();
	STDMETHOD(OnShutdown)();
	STDMETHOD(OnInitializing)();
	STDMETHOD(OnInitCompleted)();

	STDMETHOD(QueryService)(REFGUID guidService, REFIID iid, void** ppServiceObject);
	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(PluginSupport), CPluginSupport)
