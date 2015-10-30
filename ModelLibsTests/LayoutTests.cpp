// LayoutTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LayoutTests.h"

void CLayoutTests::SetUp()
{
	m_GdiplusStartupInput.GdiplusVersion = 1;
	Gdiplus::GdiplusStartup(&m_gdiPlusToken, &m_GdiplusStartupInput, NULL);

    m_wnd.Create(NULL, {0}, 0, WS_BORDER | WS_SYSMENU, WS_EX_CONTROLPARENT);
    m_wnd.ResizeClient(WIDTH1, HEIGHT1);
    CRect clientRect;
    m_wnd.GetClientRect(&clientRect);
}

void CLayoutTests::TearDown()
{
	m_wnd.DestroyWindow();
	Gdiplus::GdiplusShutdown(m_gdiPlusToken);
}

Message& operator<<(Message& os, const CRect& rect)
{
    CString str;
    str.Format(L"{%ld, %ld, %ld, %ld}", rect.left, rect.top, rect.right, rect.bottom);
    os << wstring(str);
    return os;
}

STDMETHODIMP CLayoutTests::LoadAndGetLayout(BSTR bstrLayoutName, ILayoutManager** ppLayoutManager, IVariantObject** ppLayoutObject)
{
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_LayoutManager, ppLayoutManager));
    EXPECT_NE(*ppLayoutManager, nullptr);

    CComPtr<IStream> pStream;
    EXPECT_HRESULT_SUCCEEDED(HrGetResourceStream(_AtlBaseModule.GetModuleInstance(), IDR_THEMEJSON1, L"JSON", &pStream));
    EXPECT_HRESULT_SUCCEEDED((*ppLayoutManager)->LoadThemeFromStream(pStream));

    EXPECT_HRESULT_SUCCEEDED((*ppLayoutManager)->GetLayout(bstrLayoutName, ppLayoutObject));

    return S_OK;
}

HRESULT CLayoutTests::CompareLayouts(IColumnsInfo* pColumnsInfo, vector<ResultItem>* pVector, UINT* puiIndex)
{
    UINT uiCount = 0;
    RETURN_IF_FAILED(pColumnsInfo->GetCount(&uiCount));
    for (size_t i = 0; i < uiCount; i++)
    {
        CComPtr<IColumnsInfoItem> pColumnsInfoItem;
        RETURN_IF_FAILED(pColumnsInfo->GetItem(i, &pColumnsInfoItem));
        CRect rect;
        RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&rect));
        ResultItem item = pVector->at(*puiIndex);
        EXPECT_TRUE(rect == item.rect) << item.name << L" Actual: " << rect << L" Expected: " << item.rect;
        ++(*puiIndex);
        CComPtr<IColumnsInfo> pChildren;
        RETURN_IF_FAILED(pColumnsInfoItem->GetChildItems(&pChildren));
        RETURN_IF_FAILED(CompareLayouts(pChildren, pVector, puiIndex));
    }
    return S_OK;
}

STDMETHODIMP CLayoutTests::BuildAndCompareLayout(ILayoutManager* pLayoutManager, IVariantObject* pLayoutObject, vector<ResultItem>& results)
{
    CComPtr<IImageManagerService> pImageManagerService;
    EXPECT_HRESULT_SUCCEEDED(pLayoutManager->GetImageManagerService(&pImageManagerService));

    CClientDC cdc(m_wnd.m_hWnd);
    CRect rect;
    m_wnd.GetClientRect(rect);
    CComPtr<IColumnsInfo> pColumnsInfo;
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_ColumnsInfo, &pColumnsInfo));

    EXPECT_HRESULT_SUCCEEDED(pLayoutManager->BuildLayout(cdc, &rect, pLayoutObject, nullptr, pImageManagerService, pColumnsInfo));

    UINT uiIndex = 0;
    EXPECT_HRESULT_SUCCEEDED(CompareLayouts(pColumnsInfo, &results, &uiIndex));
    EXPECT_EQ(results.size(), uiIndex);
    return S_OK;
}

