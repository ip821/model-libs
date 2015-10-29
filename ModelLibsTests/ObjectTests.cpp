#include "stdafx.h"
#include "ObjectTests.h"

TEST_F(CObjectTests, CanCreateObjects)
{
    CComPtr<IVariantObject> pVariantObject;
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
}