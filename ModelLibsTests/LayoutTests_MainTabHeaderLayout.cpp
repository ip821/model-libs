#include "stdafx.h"
#include "LayoutTests.h"

vector<ResultItem> MainTabHeaderLayout_WithoutProgress_Result =
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

vector<ResultItem> MainTabHeaderLayout_WithProgress_Result =
{
    {L"MainTabContainerHeaderLayout", CRect(0, 0, 470, 34)},
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
    { L"InfoContainer", CRect(425, 5, 465, 23) },
    { L"MarqueeProgressBox", CRect(425, 13, 465, 23) },
    { L"InfoImage", CRect(425, 5, 425, 5) }
};

TEST_F(LayoutTests, MainTabHeaderLayout_WithoutProgress)
{
    CComPtr<ILayoutManager> pLayoutManager;
    CComPtr<IVariantObject> pLayoutObject;
    EXPECT_HRESULT_SUCCEEDED(LoadAndGetLayout(L"MainTabContainerHeaderLayout", &pLayoutManager, &pLayoutObject));

    { //Setup layout
        CComPtr<IVariantObject> pInfoContainerObject;
        EXPECT_HRESULT_SUCCEEDED(HrLayoutFindItemByName(pLayoutObject, L"InfoContainer", &pInfoContainerObject));
        EXPECT_HRESULT_SUCCEEDED(HrLayoutSetVariantValueRecursive(pInfoContainerObject, L"visible", &CComVar(false)));
    }

    EXPECT_HRESULT_SUCCEEDED(BuildAndCompareLayout(pLayoutManager, pLayoutObject, MainTabHeaderLayout_WithoutProgress_Result));
}

TEST_F(LayoutTests, MainTabHeaderLayout_WithProgress)
{
    CComPtr<ILayoutManager> pLayoutManager;
    CComPtr<IVariantObject> pLayoutObject;
    EXPECT_HRESULT_SUCCEEDED(LoadAndGetLayout(L"MainTabContainerHeaderLayout", &pLayoutManager, &pLayoutObject));

    { //Setup layout
        CComPtr<IVariantObject> pInfoContainerObject;
        EXPECT_HRESULT_SUCCEEDED(HrLayoutFindItemByName(pLayoutObject, L"InfoContainer", &pInfoContainerObject));
        EXPECT_HRESULT_SUCCEEDED(HrLayoutSetVariantValueRecursive(pInfoContainerObject, L"visible", &CComVar(true)));
        pInfoContainerObject.Release();

        EXPECT_HRESULT_SUCCEEDED(HrLayoutFindItemByName(pLayoutObject, L"MarqueeProgressBox", &pInfoContainerObject));
        EXPECT_HRESULT_SUCCEEDED(HrLayoutSetVariantValueRecursive(pInfoContainerObject, L"visible", &CComVar(true)));
        pInfoContainerObject.Release();

        EXPECT_HRESULT_SUCCEEDED(HrLayoutFindItemByName(pLayoutObject, L"InfoImage", &pInfoContainerObject));
        EXPECT_HRESULT_SUCCEEDED(HrLayoutSetVariantValueRecursive(pInfoContainerObject, L"visible", &CComVar(false)));
    }

    EXPECT_HRESULT_SUCCEEDED(BuildAndCompareLayout(pLayoutManager, pLayoutObject, MainTabHeaderLayout_WithProgress_Result));
}

