// stdafx.cpp : source file that includes just the standard includes
// LayoutTests.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

HMODULE g_hObjMdl;


int main(int argc, char** argv)
{
	g_hObjMdl = LoadLibrary(L"ObjMdl.dll");
	CManualComObjectLoader loader(L"ObjMdl.dll");

	CComPtr<IPluginManager> pPluginManager;
	ASSERT_IF_FAILED(loader.CoCreateInstance(CLSID_PluginManager, NULL, IID_IPluginManager, (LPVOID*)&pPluginManager));
	ASSERT_IF_FAILED(pPluginManager->InitializePluginLibraryByName(L"ObjMdl.dll"));
	ASSERT_IF_FAILED(pPluginManager->InitializePluginLibraryByName(L"ViewMdl.dll"));
	ASSERT_IF_FAILED(pPluginManager->InitializePluginLibraryByName(L"Layout.dll"));

	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();
	std::getchar(); // keep console window open until Return keystroke
}