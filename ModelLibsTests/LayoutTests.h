#pragma once

#include "..\objmdl\Functions.h"
#include "objmdl_contract_i.h"
#include "viewmdl_contract_i.h"
#include "layout_contract_i.h"
#include "resource.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;
using namespace testing;

struct ResultItem
{
    wstring name;
    CRect rect;
};

Message& operator<<(Message& os, const CRect& rect);

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

class CLayoutTests : public testing::Test
{

protected:
	ULONG_PTR m_gdiPlusToken;
	Gdiplus::GdiplusStartupInput m_GdiplusStartupInput = { 0 };
	CTestWindow m_wnd;

	virtual void SetUp();
	virtual void TearDown();

    const int WIDTH = 470;
    const int HEIGHT1 = 900;
    const int HEIGHT2 = 240;

    STDMETHOD(LoadAndGetLayout)(BSTR bstrLayoutName, ILayoutManager** ppLayoutManager, IVariantObject** ppLayoutObject);
    STDMETHOD(BuildAndCompareLayout)(ILayoutManager* pLayoutManager, IVariantObject* pLayoutObject, vector<ResultItem>& results);
    HRESULT CompareLayouts(IColumnsInfo* pColumnsInfo, vector<ResultItem>* pVector, UINT* puiIndex);
};
