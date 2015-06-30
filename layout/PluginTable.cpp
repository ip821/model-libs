#include "stdafx.h"
#include "PluginTable.h"

HRESULT CPluginTable::FinalConstruct()
{
	RETURN_IF_FAILED(AddObject(CLSID_ImageManagerService, L"Image manager service"));
	RETURN_IF_FAILED(AddObject(CLSID_ColumnsInfo, L"Columns info object"));
	RETURN_IF_FAILED(AddObject(CLSID_ColumnsInfoItem, L"Columns info item object"));
	RETURN_IF_FAILED(AddObject(CLSID_ThemeColorMap, L"Color map object"));
	RETURN_IF_FAILED(AddObject(CLSID_ThemeFontMap, L"Font map object"));
	RETURN_IF_FAILED(AddObject(CLSID_LayoutManager, L"Layout manager object"));
	RETURN_IF_FAILED(AddObject(CLSID_LayoutBuilder, L"Layout builder object"));
	RETURN_IF_FAILED(AddObject(CLSID_LayoutPainter, L"Layout painter object"));

	return S_OK;
}
