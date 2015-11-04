// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define __NETMDL__
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlcomcli.h>
#include <atlctl.h>

#include <vector>
#include <string>

#include <curl/curl.h>
#include <curl/easy.h>
#include <Winhttp.h>

#include <initguid.h>
#include "..\ObjMdl\Plugins.h"
#include "..\ObjMdl\Macros.h"
#include "..\ObjMdl\Functions.h"
#include "..\ObjMdl\Metadata.h"
#include "Metadata.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "wldap32.lib")