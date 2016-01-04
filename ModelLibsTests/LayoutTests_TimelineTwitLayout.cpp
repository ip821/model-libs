#include "stdafx.h"
#include "LayoutTests.h"

vector<ResultItem> TimelineTwitLayoutResult = 
{
    { L"TimelineItem", CRect(0, 0, 453, 114) },
    { L"ContentContainer", CRect(0, 5, 453, 114) },
    { L"RetweetContainer", CRect(58, 5, 326, 18) },
    { L"RetweetImage", CRect(58, 5, 74, 18) },
    { L"RetweetTextStatic", CRect(74, 5, 142, 18) },
    { L"TwitterRetweetedUserDisplayName", CRect(144, 5, 236, 18) },
    { L"TwitterRetweetedUserName", CRect(236, 5, 326, 18) },
    { L"UserInfoContainer", CRect(0, 18, 453, 114) },
    { L"TwitterUserImage", CRect(5, 23, 53, 71) },
    { L"TextDataContainer", CRect(58, 18, 453, 114) },
    { L"UserNameContainer", CRect(58, 18, 453, 34) },
    { L"TwitterUserDisplayName", CRect(58, 18, 168, 34) },
    { L"TwitterUserName", CRect(173, 18, 225, 34) },
    { L"StatsContainer", CRect(225, 18, 286, 34) },
    { L"StatsRetweetContainer", CRect(235, 18, 258, 34) },
    { L"StatsRetweetContentContainer", CRect(235, 18, 258, 34) },
    { L"StatsRetweetContentImage", CRect(235, 19, 251, 32) },
    { L"StatsRetweetContentCount", CRect(251, 18, 258, 34) },
    { L"StatsFavContainer", CRect(263, 18, 286, 34) },
    { L"StatsFavContentContainer", CRect(263, 18, 286, 34) },
    { L"StatsFavContentImage", CRect(263, 19, 279, 32) },
    { L"StatsFavContentCount", CRect(279, 18, 286, 34) },
    { L"TwitterRelativeTime", CRect(376, 18, 448, 34) },
    { L"TweetText", CRect(58, 34, 370, 114) },
    { L"", CRect(58, 34, 370, 50) },
    { L"", CRect(58, 50, 71, 66) },
    { L"TwitterUrl", CRect(58, 66, 194, 82) },
    { L"", CRect(194, 66, 207, 82) },
    { L"", CRect(58, 82, 71, 98) },
    { L"", CRect(58, 98, 297, 114) },
    { L"TwitterUrl", CRect(297, 98, 326, 114) },
};

TEST_F(CLayoutTests, TimelineTwitLayout)
{
    CComPtr<ILayoutManager> pLayoutManager;
    CComPtr<IVariantObject> pLayoutObject;
    EXPECT_HRESULT_SUCCEEDED(LoadAndGetLayout(L"TimelineItem", &pLayoutManager, &pLayoutObject));

    { //Setup layout
      //CComPtr<IVariantObject> pInfoContainerObject;
      //EXPECT_HRESULT_SUCCEEDED(HrLayoutFindItemByName(pLayoutObject, L"InfoContainer", &pInfoContainerObject));
      //EXPECT_HRESULT_SUCCEEDED(HrLayoutSetVariantValueRecursive(pInfoContainerObject, L"visible", &CComVar(false)));
    }

    m_wnd.ResizeClient(WIDTH2, HEIGHT1);

    EXPECT_HRESULT_SUCCEEDED(BuildAndCompareLayout(pLayoutManager, pLayoutObject, TimelineTwitLayoutResult));
}