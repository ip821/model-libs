// LayoutTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

HMODULE g_hObjMdl;

void Setup::SetUp()
{
	g_hObjMdl = LoadLibrary(L"ObjMdl.dll");
	CManualComObjectLoader loader(L"ObjMdl.dll");

	CComPtr<IPluginManager> pPluginManager;
	ASSERT_IF_FAILED(loader.CoCreateInstance(CLSID_PluginManager, NULL, IID_IPluginManager, (LPVOID*)&pPluginManager));
	ASSERT_IF_FAILED(pPluginManager->InitializePluginLibraryByName(L"ObjMdl.dll"));
	ASSERT_IF_FAILED(pPluginManager->InitializePluginLibraryByName(L"ViewMdl.dll"));
	ASSERT_IF_FAILED(pPluginManager->InitializePluginLibraryByName(L"Layout.dll"));
}