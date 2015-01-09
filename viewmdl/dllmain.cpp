// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ViewMdl_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CViewMdlModule _AtlModule;
HINSTANCE g_hInstance;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
#ifdef _MERGE_PROXYSTUB
	if (!PrxDllMain(hInstance, dwReason, lpReserved))
		return FALSE;
#endif
	g_hInstance = hInstance;

	return _AtlModule.DllMain(dwReason, lpReserved); 
}
