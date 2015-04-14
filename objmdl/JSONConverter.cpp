#include "stdafx.h"
#include "JSONConverter.h"
#include "Functions.h"
#include "..\objmdl\Plugins.h"

HRESULT CJsonConverter::ConvertObject(const JSONObject& obj, IVariantObject* pVariantObject)
{
	for (auto& it : obj)
	{
		auto key = it.first;
		auto value = it.second;
		switch (value->GetType())
		{
		case JSONType::JSONType_Array:
		{
			CComPtr<IObjCollection> pObjCollection;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjCollection));
			RETURN_IF_FAILED(ConvertArray(value->AsArray(), pObjCollection));
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(CComBSTR(key.c_str()), &CComVariant(pObjCollection)));
			break;
		}
		case JSONType::JSONType_Bool:
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(CComBSTR(key.c_str()), &CComVariant(value->AsBool())));
			break;
		case JSONType::JSONType_Null:
			break;
		case JSONType::JSONType_Number:
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(CComBSTR(key.c_str()), &CComVariant(value->AsNumber())));
			break;
		case JSONType::JSONType_Object:
		{
			CComPtr<IVariantObject> pValueObject;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pValueObject));
			RETURN_IF_FAILED(ConvertObject(value->AsObject(), pValueObject));
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(CComBSTR(key.c_str()), &CComVariant(pValueObject)));
			break;
		}
		case JSONType::JSONType_String:
			RETURN_IF_FAILED(pVariantObject->SetVariantValue(CComBSTR(key.c_str()), &CComVariant(value->AsString().c_str())));
			break;
		}
	}
	return S_OK;
}

HRESULT CJsonConverter::ConvertArray(const JSONArray& obj, IObjCollection* pObjCollection)
{
	for (auto& it : obj)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
		RETURN_IF_FAILED(ConvertObject(it->AsObject(), pVariantObject));
		RETURN_IF_FAILED(pObjCollection->AddObject(pVariantObject));
	}
	return S_OK;
}