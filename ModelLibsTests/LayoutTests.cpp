// LayoutTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "LayoutTests.h"

void LayoutTests::SetUp()
{
	m_GdiplusStartupInput.GdiplusVersion = 1;
	Gdiplus::GdiplusStartup(&m_gdiPlusToken, &m_GdiplusStartupInput, NULL);

    m_wnd.Create(NULL, {0}, 0, WS_BORDER | WS_SYSMENU, WS_EX_CONTROLPARENT);
    m_wnd.ResizeClient(470, 900);
    CRect clientRect;
    m_wnd.GetClientRect(&clientRect);
}

void LayoutTests::TearDown()
{
	m_wnd.DestroyWindow();
	Gdiplus::GdiplusShutdown(m_gdiPlusToken);
}

STDMETHODIMP LayoutTests::LoadAndGetLayout(BSTR bstrLayoutName, ILayoutManager** ppLayoutManager, IVariantObject** ppLayoutObject)
{
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_LayoutManager, ppLayoutManager));
    EXPECT_NE(*ppLayoutManager, nullptr);

    CComPtr<IStream> pStream;
    EXPECT_HRESULT_SUCCEEDED(HrGetResourceStream(_AtlBaseModule.GetModuleInstance(), IDR_THEMEJSON1, L"JSON", &pStream));
    EXPECT_HRESULT_SUCCEEDED((*ppLayoutManager)->LoadThemeFromStream(pStream));

    EXPECT_HRESULT_SUCCEEDED((*ppLayoutManager)->GetLayout(bstrLayoutName, ppLayoutObject));

    return S_OK;
}

