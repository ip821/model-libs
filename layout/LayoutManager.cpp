#include "stdafx.h"
#include "LayoutManager.h"
#include "..\objmdl\textfile.h"
#include "JSONConverter.h"

HRESULT CLayoutManager::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeColorMap, &m_pThemeColorMap));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ThemeFontMap, &m_pThemeFontMap));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ImageManagerService, &m_pImageManagerService));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_LayoutBuilder, &m_pLayoutBuilder));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_LayoutPainter, &m_pLayoutPainter));
	RETURN_IF_FAILED(m_pLayoutBuilder->SetColorMap(m_pThemeColorMap));
	RETURN_IF_FAILED(m_pLayoutPainter->SetColorMap(m_pThemeColorMap));
	RETURN_IF_FAILED(m_pLayoutBuilder->SetFontMap(m_pThemeFontMap));
	RETURN_IF_FAILED(m_pLayoutPainter->SetFontMap(m_pThemeFontMap));
	return S_OK;
}

STDMETHODIMP CLayoutManager::GetColorMap(IThemeColorMap** ppThemeColorMap)
{
	CHECK_E_POINTER(ppThemeColorMap);
	RETURN_IF_FAILED(m_pThemeColorMap->QueryInterface(ppThemeColorMap));
	return S_OK;
}

STDMETHODIMP CLayoutManager::GetFontMap(IThemeFontMap** ppThemeFontMap)
{
	CHECK_E_POINTER(ppThemeFontMap);
	RETURN_IF_FAILED(m_pThemeFontMap->QueryInterface(ppThemeFontMap));
	return S_OK;
}

STDMETHODIMP CLayoutManager::GetLayout(BSTR bstrLayoutName, IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(bstrLayoutName);
	CHECK_E_POINTER(ppVariantObject);

	if (m_layoutsMap.find(bstrLayoutName) == m_layoutsMap.end())
		return E_INVALIDARG;

	CComPtr<IVariantObject> pLayout;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pLayout));
	RETURN_IF_FAILED(m_layoutsMap[bstrLayoutName]->CopyTo(pLayout));
	return pLayout->QueryInterface(ppVariantObject);
}

STDMETHODIMP CLayoutManager::GetImageManagerService(IImageManagerService** ppImageManagerService)
{
	CHECK_E_POINTER(ppImageManagerService);
	RETURN_IF_FAILED(m_pImageManagerService->QueryInterface(ppImageManagerService));
	return S_OK;
}

STDMETHODIMP CLayoutManager::BuildLayout(HDC hdc, RECT* pSourceRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	RETURN_IF_FAILED(m_pLayoutBuilder->BuildLayout(hdc, pSourceRect, pLayoutObject, pValueObject, pImageManagerService, pColumnInfo));
	return S_OK;
}

STDMETHODIMP CLayoutManager::EraseBackground(HDC hdc, IColumnsInfo* pColumnInfo)
{
	CHECK_E_POINTER(pColumnInfo);
	RETURN_IF_FAILED(m_pLayoutPainter->EraseBackground(hdc, pColumnInfo));
	return S_OK;
}

STDMETHODIMP CLayoutManager::PaintLayout(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName)
{
	CHECK_E_POINTER(pColumnInfo);
	RETURN_IF_FAILED(m_pLayoutPainter->PaintLayout(hdc, pImageManagerService, pColumnInfo, bstrItemName));
	return S_OK;
}

STDMETHODIMP CLayoutManager::LoadThemeFromStream(IStream* pStream)
{
	CString strFile;
	CTextFile::ReadAllTextFromStream(pStream, strFile);
	auto value = shared_ptr<JSONValue>(JSON::Parse(strFile));
	if (value == nullptr)
		return E_FAIL;

	auto rootArray = value->AsArray();

	{
		//Load color table
		auto colorsObj = rootArray[0]->AsObject();
		auto colorArray = colorsObj[L"colors"]->AsArray();
		CComPtr<IObjCollection> pColorObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pColorObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(colorArray, pColorObjCollection));
		RETURN_IF_FAILED(m_pThemeColorMap->Initialize(pColorObjCollection));
	}

	{
		//Load font table
		auto fontsTable = rootArray[1]->AsObject();
		auto fontArray = fontsTable[L"fonts"]->AsArray();
		CComPtr<IObjCollection> pFontObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pFontObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(fontArray, pFontObjCollection));
		RETURN_IF_FAILED(m_pThemeFontMap->Initialize(pFontObjCollection));
	}

	{
		//Load images table
		auto imagesTable = rootArray[2]->AsObject();
		auto imageArray = imagesTable[L"images"]->AsArray();
		CComPtr<IObjCollection> pImagesObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pImagesObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(imageArray, pImagesObjCollection));
		RETURN_IF_FAILED(m_pImageManagerService->Initialize(pImagesObjCollection));
	}

	{
		//Load styles table
		auto stylesTable = rootArray[3]->AsObject();
		auto styleArray = stylesTable[L"styles"]->AsArray();
		CComPtr<IObjCollection> pStylesObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pStylesObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(styleArray, pStylesObjCollection));

		UINT uiCount = 0;
		RETURN_IF_FAILED(pStylesObjCollection->GetCount(&uiCount));
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> pStyleObject;
			RETURN_IF_FAILED(pStylesObjCollection->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pStyleObject));
			CComVar vName;
			RETURN_IF_FAILED(pStyleObject->GetVariantValue(L"name", &vName));
			ATLASSERT(vName.vt == VT_BSTR);
			m_stylesMap[vName.bstrVal] = pStyleObject;
		}
	}

	{
		//Load layouts
		auto layoutsTable = rootArray[4]->AsObject();
		auto layoutArray = layoutsTable[L"layouts"]->AsArray();
		CComPtr<IObjCollection> pLayoutsObjCollection;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ObjectCollection, &pLayoutsObjCollection));
		RETURN_IF_FAILED(CJsonConverter::ConvertArray(layoutArray, pLayoutsObjCollection));
		RETURN_IF_FAILED(ApplyStyles(nullptr, pLayoutsObjCollection));

		UINT uiCount = 0;
		RETURN_IF_FAILED(pLayoutsObjCollection->GetCount(&uiCount));
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> pLayoutObject;
			RETURN_IF_FAILED(pLayoutsObjCollection->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pLayoutObject));
			CComVar vName;
			RETURN_IF_FAILED(pLayoutObject->GetVariantValue(L"name", &vName));
			ATLASSERT(vName.vt == VT_BSTR);
			m_layoutsMap[vName.bstrVal] = pLayoutObject;
		}
	}
	return S_OK;
}

STDMETHODIMP CLayoutManager::CopyProps(IVariantObject* pSourceObject, IVariantObject* pDestObject)
{
	UINT uiPropsCount = 0;
	RETURN_IF_FAILED(pSourceObject->GetCount(&uiPropsCount));
	for (size_t j = 0; j < uiPropsCount; j++)
	{
		CComBSTR bstrKey;
		RETURN_IF_FAILED(pSourceObject->GetKeyByIndex(j, &bstrKey));

		if (bstrKey == L"name")
			continue;

		CComVar vValue;
		RETURN_IF_FAILED(pDestObject->GetVariantValue(bstrKey, &vValue));

		if (vValue.vt == VT_EMPTY)
		{
			CComVar vProp;
			RETURN_IF_FAILED(pSourceObject->GetVariantValue(bstrKey, &vProp));
			RETURN_IF_FAILED(pDestObject->SetVariantValue(bstrKey, &vProp));
		}
	}
	return S_OK;
}

STDMETHODIMP CLayoutManager::ApplyStyles(IVariantObject* pParentObject, IObjArray* pElements)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pElements->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pElement;
		RETURN_IF_FAILED(pElements->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pElement));

		CComVar vStyle;
		RETURN_IF_FAILED(pElement->GetVariantValue(L"style", &vStyle));
		if (vStyle.vt == VT_BSTR)
		{
			RETURN_IF_FAILED(ApplyStyle(pElement, vStyle.bstrVal));
		}
		RETURN_IF_FAILED(ApplyStyle(pElement, L"")); //apply default
	}
	return S_OK;
}

STDMETHODIMP CLayoutManager::ApplyStyle(IVariantObject* pElement, BSTR bstrStyleName)
{
	auto it = m_stylesMap.find(bstrStyleName);
	if (it != m_stylesMap.end())
	{
		RETURN_IF_FAILED(CopyProps(it->second, pElement));
	}

	CComVar vChildElements;
	RETURN_IF_FAILED(pElement->GetVariantValue(L"elements", &vChildElements));
	if (vChildElements.vt == VT_UNKNOWN)
	{
		CComQIPtr<IObjArray> pChildElements = vChildElements.punkVal;
		RETURN_IF_FAILED(ApplyStyles(pElement, pChildElements));
	}
	return S_OK;
}