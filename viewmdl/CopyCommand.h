// CopyCommand.h : Declaration of the CCopyCommand

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;


// CCopyCommand

class ATL_NO_VTABLE CCopyCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCopyCommand, &CLSID_CopyCommand>,
	public ICommand,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IInitializeWithColumnName
{
public:
	CCopyCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CCopyCommand)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ICommand)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithColumnName)
	END_COM_MAP()

private:
	CComPtr<IVariantObject> m_pVariantObject;
	CString m_strColumnName;
public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CHECK_E_POINTER(bstrText);
		*bstrText = CComBSTR(L"Copy text").Copy();
		return S_OK;
	}

	STDMETHOD(InstallMenu)(IMenu* pMenu)
	{
		CHECK_E_POINTER(pMenu);
		return pMenu->AddMenuCommand(GUID_NULL, CLSID_CopyCommand, this);
	}

	STDMETHOD(Invoke)(REFGUID guidCommand)
	{
		UNREFERENCED_PARAMETER(guidCommand);
		CComVar v;
		RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(CComBSTR(m_strColumnName), &v));
		{
			if ( OpenClipboard(NULL) )
			{
				if (EmptyClipboard()) 
				{
					std::wstring str;
					switch(v.vt)
					{
					case VT_BSTR:
						str = v.bstrVal;
						break;

					case VT_I4:
						str = boost::lexical_cast<std::wstring>(v.intVal);
						break;

					case VT_I8:
						str = boost::lexical_cast<std::wstring>(v.llVal);
						break;
					}

					size_t nLength = _tcslen(str.c_str());
					size_t nByteOfBuffer = (nLength + 1) * sizeof(TCHAR);
					HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,  nByteOfBuffer);
					if (NULL == hGlobal) 
					{
						CloseClipboard();
						return S_OK;
					}

					LPTSTR pBuf = (LPTSTR)GlobalLock(hGlobal); 
					if (NULL == pBuf)
					{
						GlobalFree(hGlobal);
						hGlobal = NULL;
						return HRESULT_FROM_WIN32(GetLastError());
					}

					_tcscpy_s(pBuf, nLength + 1, str.c_str());
					SetClipboardData(CF_UNICODETEXT, hGlobal);
					GlobalUnlock(hGlobal);
					CloseClipboard();
				}
			}
		}
		return S_OK;
	}

	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject)
	{
		m_pVariantObject = pVariantObject;
		return S_OK;
	}

	STDMETHOD(SetColumnName)(LPCTSTR lpszColumnName)
	{
		m_strColumnName = lpszColumnName;
		return S_OK;
	}
};

OBJECT_ENTRY_AUTO(__uuidof(CopyCommand), CCopyCommand)
