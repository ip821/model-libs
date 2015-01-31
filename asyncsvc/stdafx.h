// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
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

#define __NOTIFICATIONSERVICES__
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#define BOOST_LIB_DIAGNOSTIC 1

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

#include <vector>
#include <hash_set>
#include <queue>
#include <map>
#include <mutex>
#include <thread>
#include <atomic>

#include <boost/thread/condition_variable.hpp>

#include <initguid.h>
#include "..\ObjMdl\Plugins.h"
#include "..\ObjMdl\Macros.h"
#include "..\ObjMdl\Functions.h"
#include "..\ObjMdl\Metadata.h"
#include "..\ViewMdl\Plugins.h"
#include "..\ViewMdl\Functions.h"
#include "Metadata.h"
