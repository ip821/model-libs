// PluginManager.h : Declaration of the CPluginManager

#pragma once
#include "resource.h"       // main symbols
#include "PluginTable.h"
#include "ObjMdl_i.h"

using namespace ATL;


// CPluginManager

class ATL_NO_VTABLE CPluginManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPluginManager, &CLSID_PluginManager>,
	public IPluginManager
{
public:
	CPluginManager()
	{
	}

	DECLARE_CLASSFACTORY_SINGLETON(CPluginManager)
	DECLARE_REGISTRY_RESOURCEID(IDR_PLUGINMANAGER)

	BEGIN_COM_MAP(CPluginManager)
		COM_INTERFACE_ENTRY(IPluginManager)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT();
	HRESULT FinalConstruct();
	void FinalRelease();

private:
	struct GUIDComparer
	{
		inline bool const operator() (const GUID & lhs, const GUID & rhs)
		{
			return ( memcmp( &lhs, &rhs, sizeof(GUID) ) > 0 ? true : false );
		}
	};

	struct InnerMap : std::map<GUID, CComPtr<IPluginInfo> , GUIDComparer> {};
	typedef std::map<GUID, std::map<GUID, InnerMap, GUIDComparer>, GUIDComparer > GuidMap;

	struct InnerList : std::list<CComPtr<IPluginInfo>> {};
	typedef std::map<GUID, std::map<GUID, InnerList, GUIDComparer>, GUIDComparer > GuidList;
	GuidMap m_pPlugins;
	GuidList m_pPluginsList;
	std::hash_set<HMODULE> m_loadedLibs;
	CString m_strPath;

	STDMETHOD(InitializeByPluginTable)(IPluginTable* pPluginTable);

public:

	STDMETHOD(GetPluginInfoCollection)(REFGUID guidNamespace, REFGUID guidType, IObjArray** ppPluginInfos);
	STDMETHOD(GetPluginInfo)(REFGUID guidNamespace, REFGUID guidType, REFGUID guidId, IPluginInfo** ppPluginInfo);
	STDMETHOD(CreatePluginInstance)(REFGUID guidNamespace, REFGUID guidType, REFGUID guidId, IUnknown** ppUnknown);
	STDMETHOD(RegisterPluginInfo)(IPluginInfo* pPluginInfo);
	STDMETHOD(RegisterPluginInfo2)(IVariantObject* pVariantObject);
	STDMETHOD(LoadPlugins)(BSTR bstrFileMask);
	STDMETHOD(InitializePluginLibraryByHandle)(HMODULE hModule);
	STDMETHOD(InitializePluginLibraryByName)(BSTR bstrDllName);
	STDMETHOD(CoCreateInstance)(REFCLSID rclsid, REFIID riid, LPVOID* lpvoid);
	STDMETHOD(SetDirectory)(BSTR bstrPath);
	STDMETHOD(Shutdown)();
};

OBJECT_ENTRY_AUTO(__uuidof(PluginManager), CPluginManager)
