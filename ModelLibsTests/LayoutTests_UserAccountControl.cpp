#include "stdafx.h"
#include "LayoutTests.h"

vector<ResultItem> UserAccountControlLayoutResult =
{
    { L"UserAccountControlLayout", CRect(0, 0, 470, 240) },
    { L"UserAccountControlLayoutBackgroundImageContainer", CRect(0, 0, 470, 240) },
    { L"UserAccountControlLayoutBackgroundImage", CRect(0, 0, 470, 240) },
    { L"UserAccountControlLayoutImageViewContainer", CRect(0, 0, 470, 240) },
    { L"UserAccountControlLayoutUserInfoContainer", CRect(119, 20, 351, 86) },
    { L"TwitterUserImageContainer", CRect(119, 24, 177, 82) },
    { L"TwitterUserImage", CRect(124, 29, 172, 77) },
    { L"UserInfoContainer", CRect(187, 20, 351, 86) },
    { L"UserDisplayName", CRect(187, 20, 291, 53) },
    { L"UserName", CRect(187, 53, 351, 86) },
    { L"UserInfoString", CRect(131, 96, 339, 128) },
    { L"BottomContainer", CRect(0, 182, 470, 240) },
    { L"CounterContainer", CRect(30, 182, 129, 230) },
    { L"CounterContainerCatpion", CRect(30, 182, 94, 230) },
    { L"TweetsCounter", CRect(30, 182, 81, 198) },
    { L"FollowingCounter", CRect(30, 198, 93, 214) },
    { L"FollowedCounter", CRect(30, 214, 94, 230) },
    { L"CounterContainerValues", CRect(94, 182, 129, 230) },
    { L"TweetsCounter", CRect(94, 182, 129, 198) },
    { L"FollowingCounter", CRect(94, 198, 115, 214) },
    { L"FollowedCounter", CRect(94, 214, 122, 230) },
    { L"FollowButtonContainer", CRect(374, 185, 460, 220) },
    { L"TwitterItemFollowButton", CRect(384, 193, 450, 212) }
};

TEST_F(LayoutTests, UserAccountControlLayout)
{
    CComPtr<ILayoutManager> pLayoutManager;
    CComPtr<IVariantObject> pLayoutObject;
    EXPECT_HRESULT_SUCCEEDED(LoadAndGetLayout(L"UserAccountControlLayout", &pLayoutManager, &pLayoutObject));

    { //Setup layout
        //CComPtr<IVariantObject> pInfoContainerObject;
        //EXPECT_HRESULT_SUCCEEDED(HrLayoutFindItemByName(pLayoutObject, L"InfoContainer", &pInfoContainerObject));
        //EXPECT_HRESULT_SUCCEEDED(HrLayoutSetVariantValueRecursive(pInfoContainerObject, L"visible", &CComVar(false)));
    }

    m_wnd.ResizeClient(WIDTH, HEIGHT2);

    EXPECT_HRESULT_SUCCEEDED(BuildAndCompareLayout(pLayoutManager, pLayoutObject, UserAccountControlLayoutResult));
}
