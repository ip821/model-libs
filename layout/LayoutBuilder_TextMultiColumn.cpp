#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "..\objmdl\StringUtils.h"

STDMETHODIMP CLayoutBuilder::BuildTextMultiColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(pLayoutObject, &pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetVariantValue(Layout::Metadata::Element::Type, &(CComVar(Layout::Metadata::LayoutTypes::TextMultiColumn))));

	CComVar vVisible;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::Element::Visible, &vVisible));
	ATLASSERT(vVisible.vt == VT_BOOL);

	if (vVisible.boolVal)
	{
		CComBSTR bstrText;
		RETURN_IF_FAILED(GetTextForTextColumn(pLayoutObject, pValueObject, &bstrText));

		CString strText(bstrText);
		vector<CString> vStrItems;
		
		int lastStartIndex = 0;
		CString item;
		auto strTextLength = strText.GetLength();
		for (int i = 0; i < strTextLength; i++)
		{
			TCHAR ch = strText[i];
			item += ch;
			if (((ch == L' ' || ch == L'\n') && lastStartIndex != i) || i == strTextLength - 1)
			{
				vStrItems.push_back(item);
				item.Empty();
			}
		}

		CRect rect = *pSourceRect;
		CRect itemRect;
		itemRect.right = itemRect.left;
		itemRect.bottom = itemRect.top;
		CString strLine;
		CString strItemFull;
		auto length = vStrItems.size();
		CComPtr<IColumnsInfo> pChildren;
		RETURN_IF_FAILED(pColumnsInfoItem->GetChildItems(&pChildren));
		int maxCx = 0;
		int maxCy = 0;
		for (size_t i = 0; i < length; i++)
		{
			CString strItem = vStrItems[i];
			strLine += CString(strItem).Trim();
			strItemFull += strItem;

			CSize szItem = { 0 };
			auto bRes = GetTextExtentPoint32(hdc, strLine, strLine.GetLength(), &szItem);
			ATLASSERT(bRes);

			TEXTMETRIC tmItem = { 0 };
			bRes = GetTextMetrics(hdc, &tmItem);
			ATLASSERT(bRes);

			szItem.cx -= tmItem.tmOverhang;

			itemRect.right = itemRect.left + szItem.cx;
			itemRect.bottom = itemRect.top + szItem.cy;

			CString strWithNextItem = strLine;
			if(i != length - 1)
				strWithNextItem += vStrItems[i + 1];

			CSize szWithNextItem = { 0 };
			bRes = GetTextExtentPoint32(hdc, strWithNextItem, strWithNextItem.GetLength(), &szWithNextItem);
			ATLASSERT(bRes);

			TEXTMETRIC tmWithNextItem = { 0 };
			bRes = GetTextMetrics(hdc, &tmWithNextItem);
			ATLASSERT(bRes);

			szWithNextItem.cx -= tmWithNextItem.tmOverhang;

			auto lastChar = strItem[strItem.GetLength() - 1];
			if (i == length - 1 || szWithNextItem.cx >= rect.Width() || lastChar == L'\n')
			{
				CComPtr<IVariantObject> pVariantObjectItem;
				RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObjectItem));
				CComPtr<IColumnsInfoItem> pItem;
				RETURN_IF_FAILED(pChildren->AddItem(pVariantObjectItem, &pItem));
				RETURN_IF_FAILED(pItem->SetRect(itemRect));
				RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(true)));
				RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::Element::Type, &CComVar(Layout::Metadata::LayoutTypes::TextColumn)));
				RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::TextColumn::TextFullKey , &CComVar(strItemFull)));
				RETURN_IF_FAILED(HrCopyProp(pColumnsInfoItem, pItem, Layout::Metadata::TextColumn::Font));
				RETURN_IF_FAILED(HrCopyProp(pColumnsInfoItem, pItem, Layout::Metadata::Element::Color));

				maxCx = max(maxCx, itemRect.right);
				maxCy = max(maxCy, itemRect.bottom);

				strLine.Empty();
				strItemFull.Empty();
				itemRect.right = itemRect.left;
				itemRect.top = itemRect.bottom;
			}
		}
		CRect rectResultRect = *pSourceRect;
		rectResultRect.right = rectResultRect.left + maxCx;
		rectResultRect.bottom = rectResultRect.top + maxCy;
		RETURN_IF_FAILED(pColumnsInfoItem->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(true)));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectResultRect));
		*pDestRect = rectResultRect;
	}

	return S_OK;
}