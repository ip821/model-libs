#include "stdafx.h"
#include "PluginInfo.h"
#include "Functions.h"
#include "VariantObject.h"

HRESULT CPluginInfo::FinalConstruct()
{
	RETURN_IF_FAILED(HrGetPluginManager(&m_pPluginManager));
	return CVariantObject::_CreatorClass::CreateInstance(NULL, IID_IVariantObject, (LPVOID*)&m_pVariantObject);
}

STDMETHODIMP CPluginInfo::GetStringValue(BSTR bstrKey, BSTR* bstr)
{
	CHECK_E_POINTER(bstr);
	CComVariant v;
	m_pVariantObject->GetVariantValue(bstrKey, &v);
	*bstr = ::SysAllocString(v.bstrVal);
	return S_OK;
}

STDMETHODIMP CPluginInfo::GetName(BSTR* bstrName)
{
	CHECK_E_POINTER(bstrName);
	return GetStringValue(Metadata::Object::Name, bstrName);
}

STDMETHODIMP CPluginInfo::GetDescription(BSTR* bstrName)
{
	CHECK_E_POINTER(bstrName);
	return GetStringValue(Metadata::Object::Description, bstrName);
}

STDMETHODIMP CPluginInfo::GetNamespace(GUID* guidNamespace)
{
	CHECK_E_POINTER(guidNamespace);
	CComVariant v;
	m_pVariantObject->GetVariantValue(IP::ObjectModel::Metadata::Plugins::Object::Namespace, &v);
	return CLSIDFromString(v.bstrVal, guidNamespace);
}

STDMETHODIMP CPluginInfo::GetType(GUID* guidType)
{
	CHECK_E_POINTER(guidType);
	CComVariant v;
	m_pVariantObject->GetVariantValue(Metadata::Plugins::Object::Type, &v);
	return CLSIDFromString(v.bstrVal, guidType);
}

STDMETHODIMP CPluginInfo::GetId(GUID* guidId)
{
	CHECK_E_POINTER(guidId);
	CComVariant v;
	m_pVariantObject->GetVariantValue(Metadata::Plugins::Object::Id, &v);
	return CLSIDFromString(v.bstrVal, guidId);
}

STDMETHODIMP CPluginInfo::GetCLSID(GUID* guidCLSID)
{
	CHECK_E_POINTER(guidCLSID);
	CComVariant v;
	m_pVariantObject->GetVariantValue(Metadata::Plugins::Object::Clsid, &v);
	return CLSIDFromString(v.bstrVal, guidCLSID);
}

STDMETHODIMP CPluginInfo::CreateInstance(IUnknown** ppUnknown)
{
	CHECK_E_POINTER(ppUnknown);
	GUID clsid = {0};
	RETURN_IF_FAILED(GetCLSID(&clsid));

	CComVariant v;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(Metadata::Plugins::Object::ClassFactoryObject, &v));
	if(v.vt == VT_UNKNOWN && v.punkVal)
	{
		CComQIPtr<IClassFactory> pClassFactory;
		pClassFactory = v.punkVal;
		RETURN_IF_FAILED(pClassFactory->CreateInstance(NULL, IID_IUnknown, (LPVOID*)ppUnknown));
	}
	else
	{
		RETURN_IF_FAILED(m_pPluginManager->CoCreateInstance(clsid, IID_IUnknown, (LPVOID*)ppUnknown));
		CComQIPtr<IInitializeWithPluginInfo> pInit = *ppUnknown;
		if(pInit)
		{
			RETURN_IF_FAILED(pInit->SetPluginInfo(this));
		}
	}

	return S_OK;
}
