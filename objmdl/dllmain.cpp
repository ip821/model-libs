// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ObjMdl_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CObjMdlModule _AtlModule;
HMODULE g_hObjMdl;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	hInstance;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hObjMdl = hInstance;
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{
#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
#endif
		g_hObjMdl = NULL;
	}


	return _AtlModule.DllMain(dwReason, lpReserved); 
}
