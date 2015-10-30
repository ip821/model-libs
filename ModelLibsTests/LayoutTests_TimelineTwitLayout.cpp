#include "stdafx.h"
#include "LayoutTests.h"

vector<ResultItem> TimelineTwitLayoutResult = 
{
    { L"TimelineItem", CRect(0, 0, 453, 114) },
    { L"ContentContainer", CRect(0, 5, 453, 114) },
    { L"RetweetContainer", CRect(58, 5, 330, 18) },
    { L"RetweetImage", CRect(58, 5, 74, 18) },
    { L"RetweetTextStatic", CRect(74, 5, 142, 18) },
    { L"TwitterRetweetedUserDisplayName", CRect(144, 5, 240, 18) },
    { L"TwitterRetweetedUserName", CRect(240, 5, 330, 18) },
    { L"UserInfoContainer", CRect(0, 18, 453, 114) },
    { L"TwitterUserImage", CRect(5, 23, 53, 71) },
    { L"TextDataContainer", CRect(58, 18, 453, 114) },
    { L"UserNameContainer", CRect(58, 18, 453, 34) },
    { L"TwitterUserDisplayName", CRect(58, 18, 186, 34) },
    { L"TwitterUserName", CRect(191, 18, 243, 34) },
    { L"StatsContainer", CRect(243, 18, 304, 34) },
    { L"StatsRetweetContainer", CRect(253, 18, 276, 34) },
    { L"StatsRetweetContentContainer", CRect(253, 18, 276, 34) },
    { L"StatsRetweetContentImage", CRect(253, 19, 269, 32) },
    { L"StatsRetweetContentCount", CRect(269, 18, 276, 34) },
    { L"StatsFavContainer", CRect(281, 18, 304, 34) },
    { L"StatsFavContentContainer", CRect(281, 18, 304, 34) },
    { L"StatsFavContentImage", CRect(281, 19, 297, 32) },
    { L"StatsFavContentCount", CRect(297, 18, 304, 34) },
    { L"TwitterRelativeTime", CRect(376, 18, 448, 34) },
    { L"TweetText", CRect(58, 34, 380, 114) },
    { L"", CRect(58, 34, 380, 50) },
    { L"", CRect(58, 50, 71, 66) },
    { L"TwitterUrl", CRect(58, 66, 194, 82) },
    { L"", CRect(194, 66, 207, 82) },
    { L"", CRect(58, 82, 71, 98) },
    { L"", CRect(58, 98, 330, 114) },
    { L"TwitterUrl", CRect(330, 98, 359, 114) },
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