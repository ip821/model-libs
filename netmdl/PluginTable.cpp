#include "stdafx.h"
#include "PluginTable.h"
#include "Plugins.h"

CPluginTable::CPluginTable(void)
{
}

HRESULT CPluginTable::FinalConstruct()
{
	AddObject(CLSID_HttpConnection, L"HttpConnection");
	return S_OK;
}
