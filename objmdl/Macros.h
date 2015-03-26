#pragma once

#include <comdef.h>

#define RETURN_IF_FAILED(_ex) { HRESULT _hr = _ex; if(FAILED(_hr)) {ATLASSERT(FALSE); return _hr;} }
#define ASSERT_IF_FAILED(_ex) { HRESULT _hr = _ex; if(FAILED(_hr)) {ATLASSERT(FALSE); } }
#define CHECK_E_POINTER(p) { if(!p) {ATLASSERT(FALSE);return E_POINTER;} }
#define MSGBOX_IF_FAILED(_ex) \
	{ \
		HRESULT _hr = _ex;\
		if (FAILED(_hr) && _hr != HRESULT_FROM_WIN32(ERROR_CANCELLED))\
		{\
		MessageBox(::GetActiveWindow(), _com_error(_hr).ErrorMessage(), L"Error", MB_ICONERROR); \
		return _hr;\
		}\
	}

#define METHOD_EMPTY(m) m{ return S_OK; };
#define DEFAULT_MESSAGE_HANDLER(func) \
		{ \
		bHandled = FALSE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
		}
