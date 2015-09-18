#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "..\objmdl\StringUtils.h"

STDMETHODIMP CLayoutBuilder::BuildTextMultiColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(pLayoutObject, &pColumnsInfoItem));

	CComVar vVisible;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::Element::Visible, &vVisible));
	ATLASSERT(vVisible.vt == VT_BOOL);

	if (vVisible.boolVal)
	{
		CComVar vText;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextColumn::Text, &vText));
		CComBSTR bstrText;
		VarToString(vText, bstrText);

		CString strText(bstrText);
		vector<CString> vStrItems;
		StrSplit(strText, L" ,.", vStrItems);

		vector<CSize> vSizes;
		for (auto& it : vStrItems)
		{
			CSize sz = { 0 };
			auto bRes = GetTextExtentPoint32(hdc, it, it.GetLength(), &sz);
			ATLASSERT(bRes);
			vSizes.push_back(sz);
		}
	}

	return S_OK;
}