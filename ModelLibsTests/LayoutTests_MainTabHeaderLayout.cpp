#include "stdafx.h"
#include "LayoutTests.h"

struct ResultItem
{
    wstring name;
    CRect rect;
};

Message& operator<<(Message& os, const CRect& rect)
{
    CString str;
    str.Format(L"{%ld, %ld, %ld, %ld}", rect.left, rect.top, rect.right, rect.bottom);
    os << wstring(str);
    return os;
}

vector<ResultItem> MainTabHeaderLayoutResult =
{
    { L"MainTabContainerHeaderLayout", CRect(0, 0, 470, 34) },
    { L"HomeContainer", CRect(10, 5, 70, 29) },
    { L"HomeImage", CRect(10, 5, 34, 29) },
    { L"HomeCaption", CRect(37, 9, 70, 25) },
    { L"ListsContainer", CRect(90, 5, 142, 29) },
    { L"ListsImage", CRect(90, 5, 114, 29) },
    { L"ListsCaption", CRect(117, 9, 142, 25) },
    { L"SearchContainer", CRect(162, 5, 229, 29) },
    { L"SearchImage", CRect(162, 5, 186, 29) },
    { L"SearchCaption", CRect(189, 9, 229, 25) },
    { L"SettingsContainer", CRect(249, 5, 322, 29) },
    { L"SettingsImage", CRect(249, 5, 273, 29) },
    { L"SettingsCaption", CRect(276, 9, 322, 25) },
    { L"InfoContainer", CRect(465, 5, 465, 5) },
    { L"InfoImage", CRect(465, 5, 465, 5) }, 
};

HRESULT CompareLayouts(IColumnsInfo* pColumnsInfo, vector<ResultItem>* pVector, UINT* puiIndex)
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

TEST_F(LayoutTests, MainTabHeaderLayout)
{
    CComPtr<ILayoutManager> pLayoutManager;
    CComPtr<IVariantObject> pLayoutObject;
    EXPECT_HRESULT_SUCCEEDED(LoadAndGetLayout(L"MainTabContainerHeaderLayout", &pLayoutManager, &pLayoutObject));

    { //Setup layout
        CComPtr<IVariantObject> pInfoContainerObject;
        EXPECT_HRESULT_SUCCEEDED(HrLayoutFindItemByName(pLayoutObject, L"InfoContainer", &pInfoContainerObject));
        EXPECT_HRESULT_SUCCEEDED(HrLayoutSetVariantValueRecursive(pInfoContainerObject, L"visible", &CComVar(false)));
    }

    CComPtr<IImageManagerService> pImageManagerService;
    EXPECT_HRESULT_SUCCEEDED(pLayoutManager->GetImageManagerService(&pImageManagerService));

    CClientDC cdc(m_wnd.m_hWnd);
    CRect rect;
    m_wnd.GetClientRect(rect);
    CComPtr<IColumnsInfo> pColumnsInfo;
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_ColumnsInfo, &pColumnsInfo));

    EXPECT_HRESULT_SUCCEEDED(pLayoutManager->BuildLayout(cdc, &rect, pLayoutObject, nullptr, pImageManagerService, pColumnsInfo));

    UINT uiIndex = 0;
    EXPECT_HRESULT_SUCCEEDED(CompareLayouts(pColumnsInfo, &MainTabHeaderLayoutResult, &uiIndex));
}

