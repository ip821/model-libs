#include "stdafx.h"
#include "PluginTable.h"


CPluginTable::CPluginTable(void)
{
}

HRESULT CPluginTable::FinalConstruct()
{
	RETURN_IF_FAILED(AddObject(CLSID_ObjectCollection, _T("Object collection")));
	RETURN_IF_FAILED(AddObject(CLSID_VariantObject, _T("Variant object")));
	RETURN_IF_FAILED(AddObject(CLSID_VariantTable, _T("Variant object table")));
	RETURN_IF_FAILED(AddObject(CLSID_RegistrySettings, _T("Registry settings")));
	RETURN_IF_FAILED(AddObject(CLSID_BstrCollection, _T("String collection")));
	RETURN_IF_FAILED(AddObject(CLSID_VariantCollection, _T("Variant collection")));
	return S_OK;
}
