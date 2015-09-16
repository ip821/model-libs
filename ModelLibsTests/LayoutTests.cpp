// LayoutTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LayoutTests.h"

#include "objmdl_contract_i.h"
#include "viewmdl_contract_i.h"
#include "layout_contract_i.h"

void LayoutTests::SetUp()
{
	m_GdiplusStartupInput.GdiplusVersion = 1;
	Gdiplus::GdiplusStartup(&m_gdiPlusToken, &m_GdiplusStartupInput, NULL);

	CRect rect(0, 0, 200, 200);
	m_wnd.Create(NULL, rect, 0, WS_BORDER | WS_SYSMENU, WS_EX_CONTROLPARENT);
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

TEST_F(LayoutTests, Test2)
{
	CComPtr<ILayoutManager> pLayoutManager;
	EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_LayoutManager, &pLayoutManager));
	EXPECT_NE(pLayoutManager, nullptr);
}