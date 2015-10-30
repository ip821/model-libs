#include "stdafx.h"
#include "ObjectTests.h"

HRESULT CompareComVar(CComVar* pv1, CComVar* pv2)
{
    if (*pv1 == *pv2)
        return S_OK;

    return E_INVALIDARG;
}

template <class T>
HRESULT TestValueCanBeStored(IVariantObject* pVariantObject, T val)
{
    CComVar v1(val);
    RETURN_IF_FAILED(pVariantObject->SetVariantValue(L"test", &v1));

    CComVar v2;
    RETURN_IF_FAILED(pVariantObject->GetVariantValue(L"test", &v2));
    RETURN_IF_FAILED(CompareComVar(&v1, &v2));
    return S_OK;
}

TEST_F(CObjectTests, CanCreateObjects)
{
    CComPtr<IVariantObject> pVariantObject;
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
    CComPtr<IObjCollection> pObjectCollection;
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_ObjectCollection, &pObjectCollection));
}

TEST_F(CObjectTests, VariantObjectGetSetValues)
{
    CComPtr<IVariantObject> pVariantObject;
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
    EXPECT_HRESULT_SUCCEEDED(TestValueCanBeStored(pVariantObject, L"test_string"));
    EXPECT_HRESULT_SUCCEEDED(TestValueCanBeStored(pVariantObject, (int)1));
    EXPECT_HRESULT_SUCCEEDED(TestValueCanBeStored(pVariantObject, (long)1));
    EXPECT_HRESULT_SUCCEEDED(TestValueCanBeStored(pVariantObject, (long long)1));
    EXPECT_HRESULT_SUCCEEDED(TestValueCanBeStored(pVariantObject, (short)1));
    EXPECT_HRESULT_SUCCEEDED(TestValueCanBeStored(pVariantObject, (BYTE)1));
    EXPECT_HRESULT_SUCCEEDED(TestValueCanBeStored(pVariantObject, true));
}

TEST_F(CObjectTests, VariantObjectCopyTo)
{
    CComPtr<IVariantObject> pVariantObject1;
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject1));
    EXPECT_HRESULT_SUCCEEDED(pVariantObject1->SetVariantValue(L"test", &CComVar(L"test_string")));

    CComPtr<IVariantObject> pVariantObject2;
    EXPECT_HRESULT_SUCCEEDED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject2));
    EXPECT_HRESULT_SUCCEEDED(pVariantObject1->CopyTo(pVariantObject2));

    EXPECT_HRESULT_SUCCEEDED(pVariantObject1->SetVariantValue(L"test", &CComVar(L"test_string2")));

    CComVar v1;
    EXPECT_HRESULT_SUCCEEDED(pVariantObject1->GetVariantValue(L"test", &v1));

    CComVar v2;
    EXPECT_HRESULT_SUCCEEDED(pVariantObject2->GetVariantValue(L"test", &v2));
    EXPECT_HRESULT_FAILED(CompareComVar(&v1, &v2));
}