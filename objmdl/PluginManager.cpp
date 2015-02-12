// PluginManager.cpp : Implementation of CPluginManager

#include "stdafx.h"
#include "PluginManager.h"
#include "PluginInfo.h"

// CPluginManager

HRESULT CPluginManager::FinalConstruct()
{
	return S_OK;
}

void CPluginManager::FinalRelease()
{

}

STDMETHODIMP CPluginManager::SetDirectory(BSTR bstrPath)
{
	m_strPath = bstrPath;
	return S_OK;
}

STDMETHODIMP CPluginManager::CoCreateInstance(REFCLSID rclsid, REFIID riid, LPVOID* lpvoid)
{
	CHECK_E_POINTER(lpvoid);
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(CreatePluginInstance(RAWOBJECT, RAWOBJECT, rclsid, &pUnk));
	return pUnk->QueryInterface(riid, lpvoid);
}

STDMETHODIMP CPluginManager::RegisterPluginInfo(IPluginInfo* pPluginInfo)
{
	CHECK_E_POINTER(pPluginInfo);
	GUID guidNamespace = {0};
	RETURN_IF_FAILED(pPluginInfo->GetNamespace(&guidNamespace));
	GUID guidType = {0};
	RETURN_IF_FAILED(pPluginInfo->GetType(&guidType));
	GUID guidId = {0};
	RETURN_IF_FAILED(pPluginInfo->GetId(&guidId));

	CComPtr<IPluginInfo> pPluginInfoPtr = pPluginInfo;

	m_pPlugins[guidNamespace][guidType][guidId] = pPluginInfoPtr;
	m_pPluginsList[guidNamespace][guidType].push_back(pPluginInfoPtr);
	return S_OK;
}

STDMETHODIMP CPluginManager::LoadPlugins(BSTR bstrFileMask)
{
	if(bstrFileMask != NULL)
		return E_NOTIMPL;

	TCHAR currPath[MAX_PATH] = {0};
	if (m_strPath.IsEmpty())
	{
		auto res = GetCurrentDirectory(MAX_PATH, currPath);
		if (!res || res >= MAX_PATH)
			return HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
		CString::CopyChars(currPath, MAX_PATH, m_strPath, m_strPath.GetLength());
	}

	TCHAR currPathMask[MAX_PATH] = {0};
	if(!PathCombine(currPathMask, currPath, L"*.dll"))
		return HRESULT_FROM_WIN32(GetLastError());

	WIN32_FIND_DATA ffd = {0};
	CFindFileHandle hFind(FindFirstFile(currPathMask, &ffd));
	if(hFind == INVALID_HANDLE_VALUE)
		return HRESULT_FROM_WIN32(GetLastError());

	BOOL lastFile = FALSE;
	while(!lastFile)
	{
		if(CString(PathFindExtension(ffd.cFileName)).MakeLower() == L".dll")
		{
			TCHAR dllPath[MAX_PATH] = { 0 };
			CString::CopyChars(dllPath, MAX_PATH, m_strPath, m_strPath.GetLength());
			PathAppend(dllPath, ffd.cFileName);
			HMODULE hModule = LoadLibrary(dllPath);
			if(!hModule)
			{
				return HRESULT_FROM_WIN32(GetLastError());
			}

			HRESULT hr = InitializePluginLibraryByHandle(hModule);
			if(FAILED(hr) && hr != CLASS_E_CLASSNOTAVAILABLE && hr != HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND))
			{
				return hr;
			}
		}

		if(!FindNextFile(hFind, &ffd))
		{
			DWORD dwError = GetLastError();
			if(dwError == ERROR_NO_MORE_FILES)
			{
				lastFile = TRUE;
				continue;
			}

			return HRESULT_FROM_WIN32(GetLastError());
		}
	}

	return S_OK;
}

STDMETHODIMP CPluginManager::InitializePluginLibraryByHandle(HMODULE hModule)
{
	CHECK_E_POINTER(hModule);
	if(m_loadedLibs.find(hModule) != m_loadedLibs.end())
		return S_OK;

	CManualComObjectLoader loader(hModule);

	CComPtr<IPluginTable> pPluginTable;
	HRESULT hr = loader.CoCreateInstance(CLSID_PluginTable, NULL, IID_IPluginTable, (LPVOID*)&pPluginTable);
	if (FAILED(hr))
		return hr;

	RETURN_IF_FAILED(pPluginTable->Initialize(g_hObjMdl));
	RETURN_IF_FAILED(InitializeByPluginTable(pPluginTable));
	m_loadedLibs.insert(hModule);
	return S_OK;
}

STDMETHODIMP CPluginManager::InitializeByPluginTable(IPluginTable* pPluginTable)
{
	CHECK_E_POINTER(pPluginTable);
	CComPtr<IObjArray> pObjectArray;
	HRESULT hr = pPluginTable->GetPluginTable(&pObjectArray);
	if(FAILED(hr))
		return hr;

	UINT cObjects = 0;
	hr = pObjectArray->GetCount(&cObjects);
	if(FAILED(hr))
		return hr;

	for(UINT i = 0; i < cObjects; i++)
	{
		CComPtr<IVariantObject> pVariantObjectSource;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, IID_IVariantObject, (LPVOID*)&pVariantObjectSource));
		RETURN_IF_FAILED(RegisterPluginInfo2(pVariantObjectSource));
	}

	return S_OK;
}

STDMETHODIMP CPluginManager::RegisterPluginInfo2(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComPtr<IPluginInfo> pPluginInfo;
	RETURN_IF_FAILED(CPluginInfo::_CreatorClass::CreateInstance(NULL, IID_IPluginInfo, (LPVOID*)&pPluginInfo));
	CComQIPtr<IVariantObject> pVariantObjectDest = pPluginInfo;
	ATLASSERT(pVariantObjectDest);
	RETURN_IF_FAILED(pVariantObject->CopyTo(pVariantObjectDest));
	RETURN_IF_FAILED(RegisterPluginInfo(pPluginInfo));
	return S_OK;
}

STDMETHODIMP CPluginManager::InitializePluginLibraryByName(BSTR bstrDllName)
{
	CHECK_E_POINTER(bstrDllName);
	TCHAR path[MAX_PATH] = {0};
	CString::CopyChars(path, MAX_PATH, m_strPath, m_strPath.GetLength());
	PathAppend(path, bstrDllName);
	HMODULE hModule = LoadLibrary(path);
	if(hModule == NULL)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}
	return InitializePluginLibraryByHandle(hModule);
}

STDMETHODIMP CPluginManager::CreatePluginInstance(REFGUID guidNamespace, REFGUID guidType, REFGUID guidId, IUnknown** ppUnknown)
{
	CHECK_E_POINTER(ppUnknown);
	CComPtr<IPluginInfo> pPluginInfo;
	HRESULT hr = GetPluginInfo(guidNamespace, guidType, guidId, &pPluginInfo);
	if(FAILED(hr))
		return hr;

	return pPluginInfo->CreateInstance(ppUnknown);
}

STDMETHODIMP CPluginManager::GetPluginInfo(REFGUID guidNamespace, REFGUID guidType, REFGUID guidId, IPluginInfo** ppPluginInfo)
{
	CHECK_E_POINTER(ppPluginInfo);
	auto it1 = m_pPlugins.find(guidNamespace);
	if(it1 == m_pPlugins.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	auto it2 = it1->second.find(guidType);
	if(it2 == it1->second.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	auto it3 = it2->second.find(guidId);
	if(it3 == it2->second.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	
	*ppPluginInfo = it3->second;
	(*ppPluginInfo)->AddRef();
	return S_OK;
}

STDMETHODIMP CPluginManager::GetPluginInfoCollection(REFGUID guidNamespace, REFGUID guidType, IObjArray** ppPluginInfos)
{
	CHECK_E_POINTER(ppPluginInfos);
	auto it1 = m_pPluginsList.find(guidNamespace);
	if(it1 == m_pPluginsList.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	auto it2 = it1->second.find(guidType);
	if(it2 == it1->second.end())
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	CComPtr<IObjCollection> pObjectCollection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollection));

	for(auto it = it2->second.begin(); it != it2->second.end(); it++)
	{
		RETURN_IF_FAILED(pObjectCollection->AddObject(*it));
	}

	return pObjectCollection->QueryInterface(IID_IObjArray, (LPVOID*)ppPluginInfos);
}

STDMETHODIMP CPluginManager::Shutdown()
{
	m_pPlugins.clear();
	m_pPluginsList.clear();
	return S_OK;
}