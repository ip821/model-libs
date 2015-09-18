#pragma once

using namespace ATL;
using namespace std;
using namespace Gdiplus;

class CTestWindow : public CWindowImpl<CTestWindow>
{
public:
	DECLARE_WND_CLASS(L"TestWindow")

	CTestWindow() {}
	~CTestWindow() {}

	BEGIN_MSG_MAP(CTestWindow)
	END_MSG_MAP()
};

extern CTestWindow g_wnd;

class LayoutTests : public testing::Test
{
protected:
	ULONG_PTR m_gdiPlusToken;
	Gdiplus::GdiplusStartupInput m_GdiplusStartupInput = { 0 };
	CTestWindow m_wnd;

	virtual void SetUp();
	virtual void TearDown();
};
