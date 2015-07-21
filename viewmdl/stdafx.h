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
#define _WIN32_IE	_WIN32_IE_IE70
#define _RICHEDIT_VER	0x0200
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define __VIEWMDL__
#define _WTL_NO_CSTRING

#define BOOST_LIB_DIAGNOSTIC 1

#include "resource.h"
#include <atlstr.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlcom.h>
#include <atlctl.h>


#include <atlhost.h>
#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlmisc.h>
#include <comdef.h>

#include <map>
#include <unordered_map>
#include <list>
#include <queue>

#include <boost/lexical_cast.hpp>

using namespace ATL;

#include "NTray.h"

#include <initguid.h>
#include "..\ObjMdl\Macros.h"
#include "..\ObjMdl\Metadata.h"
#include "..\ObjMdl\ManualComObjectLoader.h"
#include "..\ObjMdl\Plugins.h"
#include "..\ObjMdl\Functions.h"

#include "Plugins.h"
#include "FileCommand.h"
#include "ExitCommand.h"
#include "ViewCommand.h"
#include "ToolsCommand.h"
#include "HelpCommand.h"
#include "AboutCommand.h"
#include "TabCloseCommand.h"

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


#pragma comment(lib, "version")