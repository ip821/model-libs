// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _WTL_NO_CSTRING
#include <atlbase.h>
#include <atlstr.h>

#include <atlcom.h>
#include <atlhost.h>
#include <atlwin.h>
#include <atlctl.h>

#include <atlapp.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlmisc.h>

// TODO: reference additional headers your program requires here

#include <gdiplus.h>

#include "gtest/gtest.h"

#include <initguid.h>
#include "..\objmdl\Metadata.h"
#include "..\objmdl\Plugins.h"
#include "..\objmdl\Macros.h"
#include "..\objmdl\Functions.h"
#include "..\objmdl\ManualComObjectLoader.h"

#include "..\viewmdl\Plugins.h"
#include "..\viewmdl\Functions.h"

#include "..\layout\Metadata.h"
#include "..\layout\Plugins.h"
#include "..\layout\Functions.h"

using namespace std;
using namespace IP;

#pragma comment(lib, "gtest.lib")
#pragma comment (lib,"Gdiplus.lib")

