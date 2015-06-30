#pragma once

#include "..\json\src\JSONValue.h"
#include "objmdl_contract_i.h"

class CJsonConverter
{
public:
	static HRESULT ConvertObject(const JSONObject& obj, IVariantObject* pVariantObject);
	static HRESULT ConvertArray(const JSONArray& obj, IObjCollection* pObjCollection);
};