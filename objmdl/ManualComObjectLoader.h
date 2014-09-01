#pragma once

typedef HRESULT (__stdcall *pt_DllGetClassObject)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);

#include <atlstr.h>

using namespace ATL;

class CManualComObjectLoader
{
private:
	HMODULE m_hModule;
	CString m_strDllName;

public:
	CManualComObjectLoader(LPCTSTR lpszDllName)
	{
		m_hModule = NULL;
		m_strDllName = lpszDllName;
	}

	CManualComObjectLoader(HMODULE hModule)
	{
		m_hModule = hModule;
	}

	HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter, REFIID riid, LPVOID *ppv)
	{
		if(m_hModule == NULL)
		{
			m_hModule = LoadLibrary(m_strDllName);
			if(m_hModule == NULL)
			{
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		pt_DllGetClassObject pGCO = (pt_DllGetClassObject)::GetProcAddress(m_hModule, "DllGetClassObject");
		if(pGCO == NULL)
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		CComPtr<IClassFactory> pFactory;
		HRESULT hr = pGCO(rclsid, IID_IClassFactory, (LPVOID*)&pFactory);
		if(FAILED(hr)) 
			return hr;

		hr = pFactory->CreateInstance(pUnkOuter, riid, ppv);
		return hr;
	}

	~CManualComObjectLoader()
	{
		if(m_hModule != NULL)
		{
			//TODO: for now it is does not work, because PluginManager does not hold reference for this class
			//FreeLibrary(m_hModule);
			m_hModule = NULL;
		}
	}
};
