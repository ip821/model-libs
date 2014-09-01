#include "stdafx.h"
#include "PluginTable.h"
#include "Plugins.h"

CPluginTable::CPluginTable(void)
{
}

HRESULT CPluginTable::FinalConstruct()
{
	AddObject(CLSID_TimerService, L"Timer service object");
	AddObject(CLSID_ThreadService, L"Thread service object");
	AddObject(CLSID_ThreadPoolService, L"Thread service object");
	return S_OK;
}
