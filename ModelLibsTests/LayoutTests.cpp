// LayoutTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LayoutTests.h"

#include "..\objmdl\Functions.h"

#include "objmdl_contract_i.h"
#include "viewmdl_contract_i.h"
#include "layout_contract_i.h"
#include "resource.h"

void LayoutTests::SetUp()
{
	m_GdiplusStartupInput.GdiplusVersion = 1;
	Gdiplus::GdiplusStartup(&m_gdiPlusToken, &m_GdiplusStartupInput, NULL);

    m_wnd.Create(NULL, {0}, 0, WS_BORDER | WS_SYSMENU, WS_EX_CONTROLPARENT);
    m_wnd.ResizeClient(200, 200);
    CRect clientRect;
    m_wnd.GetClientRect(&clientRect);
}

void LayoutTests::TearDown()
{
	m_wnd.DestroyWindow();
	Gdiplus::GdiplusShutdown(m_gdiPlusToken);
}

TEST_F(LayoutTests, CreateLayoutManager)
{
	CComPtr<ILayoutManager> pLayoutManager;
	EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_LayoutManager, &pLayoutManager));
	EXPECT_NE(pLayoutManager, nullptr);
}

TEST_F(LayoutTests, LoadLayoutFile)
{
	CComPtr<ILayoutManager> pLayoutManager;
	EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_LayoutManager, &pLayoutManager));
	EXPECT_NE(pLayoutManager, nullptr);

	CComPtr<IStream> pStream;
	EXPECT_HRESULT_SUCCEEDED(HrGetResourceStream(_AtlBaseModule.GetModuleInstance(), IDR_THEMEJSON1, L"JSON", &pStream));
	EXPECT_HRESULT_SUCCEEDED(pLayoutManager->LoadThemeFromStream(pStream));
}

TEST_F(LayoutTests, GetLayoutObject)
{
	CComPtr<ILayoutManager> pLayoutManager;
	EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_LayoutManager, &pLayoutManager));
	EXPECT_NE(pLayoutManager, nullptr);

	CComPtr<IStream> pStream;
	EXPECT_HRESULT_SUCCEEDED(HrGetResourceStream(_AtlBaseModule.GetModuleInstance(), IDR_THEMEJSON1, L"JSON", &pStream));
	EXPECT_HRESULT_SUCCEEDED(pLayoutManager->LoadThemeFromStream(pStream));

	CComPtr<IVariantObject> pLayoutObject;
	EXPECT_HRESULT_SUCCEEDED(pLayoutManager->GetLayout(L"TextMultiColumn1Container", &pLayoutObject));
}

TEST_F(LayoutTests, BuildTextMultiColumn)
{
	CComPtr<ILayoutManager> pLayoutManager;
	EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_LayoutManager, &pLayoutManager));
	EXPECT_NE(pLayoutManager, nullptr);

	CComPtr<IStream> pStream;
	EXPECT_HRESULT_SUCCEEDED(HrGetResourceStream(_AtlBaseModule.GetModuleInstance(), IDR_THEMEJSON1, L"JSON", &pStream));
	EXPECT_HRESULT_SUCCEEDED(pLayoutManager->LoadThemeFromStream(pStream));

	CComPtr<IVariantObject> pLayoutObject;
	EXPECT_HRESULT_SUCCEEDED(pLayoutManager->GetLayout(L"TextMultiColumn1Container", &pLayoutObject));
	
	CComPtr<IImageManagerService> pImageManagerService;
	EXPECT_HRESULT_SUCCEEDED(pLayoutManager->GetImageManagerService(&pImageManagerService));

	CClientDC cdc(m_wnd.m_hWnd);
	CRect rect;
	m_wnd.GetClientRect(rect);
	CComPtr<IColumnsInfo> pColumnsInfo;
	EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_ColumnsInfo, &pColumnsInfo));

	EXPECT_HRESULT_SUCCEEDED(pLayoutManager->BuildLayout(cdc, &rect, pLayoutObject, nullptr, pImageManagerService, pColumnsInfo));

	UINT uiCount = 0;
	EXPECT_HRESULT_SUCCEEDED(pColumnsInfo->GetCount(&uiCount));
	EXPECT_EQ(uiCount, 1);

	CComPtr<IColumnsInfoItem> pItemContainer;
	EXPECT_HRESULT_SUCCEEDED(pColumnsInfo->GetItem(0, &pItemContainer));
	EXPECT_NE(pItemContainer, nullptr);

	CComPtr<IColumnsInfo> pContainerChildren;
	EXPECT_HRESULT_SUCCEEDED(pItemContainer->GetChildItems(&pContainerChildren));
	EXPECT_NE(pContainerChildren, nullptr);

	UINT uiChildrenCount = 0;
	EXPECT_HRESULT_SUCCEEDED(pContainerChildren->GetCount(&uiChildrenCount));
	EXPECT_EQ(uiChildrenCount, 1);

	CComPtr<IColumnsInfoItem> pMultiTextColumnItem;
	EXPECT_HRESULT_SUCCEEDED(pContainerChildren->GetItem(0, &pMultiTextColumnItem));

	CComPtr<IColumnsInfo> pMultiTextColumnChildren;
	EXPECT_HRESULT_SUCCEEDED(pMultiTextColumnItem->GetChildItems(&pMultiTextColumnChildren));

	UINT uiMultiTextColumnChildrenCount = 0;
	EXPECT_HRESULT_SUCCEEDED(pMultiTextColumnChildren->GetCount(&uiMultiTextColumnChildrenCount));
	EXPECT_NE(uiMultiTextColumnChildrenCount, 0);
	EXPECT_GT(uiMultiTextColumnChildrenCount, (UINT)1);
}