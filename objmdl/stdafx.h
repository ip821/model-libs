// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifdef __WINXP__
#define WINVER		0x0501
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200
#else
#define WINVER		0x0600
#define _WIN32_WINNT	0x0601
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define _HAS_ITERATOR_DEBUGGING 0
#define __OBJMDL__

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlstr.h>
#include <comdef.h>

#include <hash_set>
#include <map>
#include <list>

#include "FindFileHandle.h"
#include "ManualComObjectLoader.h"

#include <initguid.h>
#include "Macros.h"
#include "Plugins.h"
// {6EA78630-DC6D-4D2B-91F0-686B260F29D6}
//DEFINE_GUID(CLSID_PluginTable, 0x6ea78630, 0xdc6d, 0x4d2b, 0x91, 0xf0, 0x68, 0x6b, 0x26, 0xf, 0x29, 0xd6);
