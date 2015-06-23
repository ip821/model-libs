#pragma once

#include <atlcom.h>
#include <list>

#include "Macros.h"
#include "GUIDComparer.h"
#include "ManualComObjectLoader.h"
#include "objmdl_contract_i.h"
#include "Plugins.h"
#include "Shobjidl.h"
#include "Functions.h"

#ifdef __OBJMDL__
#include "ObjectCollection.h"
#endif

extern HINSTANCE g_hInstance;
extern HMODULE g_hObjMdl;

using namespace IP::ObjectModel;

class CPluginTableBase : public IPluginTable
{
protected:
	CManualComObjectLoader m_loader;
	std::list<CComPtr<IVariantObject>> m_infos;

private:
	STDMETHOD(AddPluginInfoInternal)(GUID gNamespace, GUID gType, GUID gId, GUID clsid, BOOL bLocalObject, LPCTSTR lpszName, LPCTSTR lpszDescription = NULL)
	{
		CComPtr<IVariantObject> pVariantObject;
		HRESULT hr = m_loader.CoCreateInstance(CLSID_VariantObject, NULL, IID_IVariantObject, (LPVOID*)&pVariantObject);
		if(FAILED(hr))
			return hr;

		CComPtr<IClassFactory> pClassFactory;
		if(bLocalObject)
		{
			RETURN_IF_FAILED(DllGetClassObject(clsid, IID_IClassFactory, (LPVOID*)&pClassFactory));
		}

		m_infos.push_back(pVariantObject);

		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Metadata::Plugins::Object::Namespace, &CComVar(gNamespace)));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Metadata::Plugins::Object::Type, &CComVar(gType)));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Metadata::Plugins::Object::Id, &CComVar(gId)));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Metadata::Plugins::Object::Clsid, &CComVar(clsid)));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Metadata::Object::Name, &CComVar(lpszName)));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Metadata::Object::Description, &CComVar(lpszDescription)));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Metadata::Plugins::Object::ClassFactoryObject, &CComVar(pClassFactory)));

		return S_OK;
	}

public:
	CPluginTableBase() : m_loader(L"ObjMdl.dll")
	{
	}

	STDMETHOD(Initialize)(HMODULE hObjMdl)
	{
		g_hObjMdl = hObjMdl;
		return S_OK;
	}

	STDMETHOD(AddObject)(REFGUID clsid, LPCTSTR lpszName, LPCTSTR lpszDescription = NULL)
	{
		return AddPluginInfoInternal(RAWOBJECT, RAWOBJECT, clsid, clsid, TRUE, lpszName, lpszDescription);
	}

	STDMETHOD(AddPluginInfoRemote)(GUID gNamespace, GUID gType, GUID gId, GUID clsid, LPCTSTR lpszName, LPCTSTR lpszDescription = NULL)
	{
		RETURN_IF_FAILED(AddPluginInfoInternal(gNamespace, gType, gId, clsid, FALSE, lpszName, lpszDescription));
		return S_OK;
	}

	STDMETHOD(AddPluginInfo)(GUID gNamespace, GUID gType, GUID gId, GUID clsid, LPCTSTR lpszName, LPCTSTR lpszDescription = NULL)
	{
		RETURN_IF_FAILED(AddObject(clsid, lpszName, lpszDescription));
		RETURN_IF_FAILED(AddPluginInfoRemote(gNamespace, gType, gId, clsid, lpszName, lpszDescription));
		return S_OK;
	}

	STDMETHOD(GetPluginTable)(IObjArray **ppObjectArray)
	{
		CComPtr<IObjCollection> pObjectCollection;
#ifdef __OBJMDL__
#include "ObjectCollection.h"
		RETURN_IF_FAILED(CObjectCollection::_CreatorClass::CreateInstance(NULL, IID_IObjCollection, (LPVOID*)&pObjectCollection));
#else
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollection));
#endif

		for(auto it = m_infos.begin(); it != m_infos.end(); it++)
		{
			RETURN_IF_FAILED(pObjectCollection->AddObject(*it));
		}

		return pObjectCollection->QueryInterface(IID_IObjArray, (LPVOID*)ppObjectArray);
	}
};
