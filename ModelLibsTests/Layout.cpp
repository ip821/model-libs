#include "stdafx.h"

#include "objmdl_contract_i.h"
#include "viewmdl_contract_i.h"
#include "layout_contract_i.h"

TEST_F(Setup, Simple)
{
	CComPtr<ILayoutManager> pLayoutManager;
	EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_LayoutManager, &pLayoutManager));
	EXPECT_NE(pLayoutManager, nullptr);
}