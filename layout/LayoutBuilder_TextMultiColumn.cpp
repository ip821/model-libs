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
		
		CComVar vColumnDefinition;
		RETURN_IF_FAILED(pValueObject->GetVariantValue(Layout::Metadata::TextMultiColumn::ColumnDefinitions, &vColumnDefinition));
		ATLASSERT(vColumnDefinition.vt == VT_UNKNOWN);
		CComQIPtr<IObjCollection> pObjCollection = vColumnDefinition.punkVal;

		vector<ColumnDefinition> columnDefinitions;
		UINT uiCount = 0;
		RETURN_IF_FAILED(pObjCollection->GetCount(&uiCount));
		for (size_t i = 0; i < uiCount; i++)
		{
			CComPtr<IVariantObject> pItem;
			RETURN_IF_FAILED(pObjCollection->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pItem));
			CComVar vStartIndex;
			RETURN_IF_FAILED(pItem->GetVariantValue(Layout::Metadata::TextMultiColumn::ColumnStartIndex, &vStartIndex));
			CComVar vEndIndex;
			RETURN_IF_FAILED(pItem->GetVariantValue(Layout::Metadata::TextMultiColumn::ColumnEndIndex, &vEndIndex));
			CComVar vColumnDef;
			RETURN_IF_FAILED(pItem->GetVariantValue(Layout::Metadata::TextMultiColumn::ColumnDefinition, &vColumnDef));
			ATLASSERT(vStartIndex.vt == VT_I4 && vEndIndex.vt == VT_I4 && vColumnDef.vt == VT_UNKNOWN);
			ColumnDefinition cd;
			cd.Start = vStartIndex.intVal;
			cd.End = vEndIndex.intVal;
			cd.ColumnObject = vColumnDef.punkVal;
			columnDefinitions.push_back(cd);
		}

		int lastStartIndex = 0;
		CString item;
		auto strTextLength = strText.GetLength();
		for (int i = 0; i < strTextLength; i++)
		{
			TCHAR ch = strText[i];
			item += ch;

			auto it = find_if(columnDefinitions.begin(), columnDefinitions.end(), [&](ColumnDefinition columnDef)
			{
				return i >= columnDef.Start && i <= columnDef.End;
			});

			auto itNext = find_if(columnDefinitions.begin(), columnDefinitions.end(), [&](ColumnDefinition columnDef)
			{
				return (i + 1) >= columnDef.Start && (i + 1) <= columnDef.End;
			});

			if (it != columnDefinitions.end() && it == itNext && i != strTextLength - 1)
				continue;

			if (((ch == L' ' || ch == L'\n') && lastStartIndex != i) || i == strTextLength - 1 || it != itNext)
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
		int width = rect.Width();
		int totalLength = 0;
		for (size_t i = 0; i < length; i++)
		{
			CString strItem = vStrItems[i];
			strLine += CString(strItem).Trim();
			strItemFull += strItem;

			CSize szItem = { 0 };
			auto bRes = GetTextExtentPoint32(hdc, strLine, strLine.GetLength(), &szItem);
			ATLASSERT(bRes);

			auto it = find_if(columnDefinitions.begin(), columnDefinitions.end(), [&](ColumnDefinition columnDef)
			{
				auto itemLength = strItem.GetLength() - 1;
				return totalLength == columnDef.Start && (itemLength + totalLength) == columnDef.End;
			});

			itemRect.right = itemRect.left + szItem.cx;
			itemRect.bottom = itemRect.top + szItem.cy;

			CString strWithNextItem = strLine;
			CString strWithNextItemFull = strItemFull;
			if (i != length - 1)
			{
				strWithNextItem += CString(vStrItems[i + 1]).Trim();
				strWithNextItemFull += vStrItems[i + 1];
			}

			CSize szWithNextItem = { 0 };
			bRes = GetTextExtentPoint32(hdc, strWithNextItem, strWithNextItem.GetLength(), &szWithNextItem);
			ATLASSERT(bRes);

			auto itWithItemNext = columnDefinitions.end();
			if (i != length - 1)
			{
				itWithItemNext = find_if(columnDefinitions.begin(), columnDefinitions.end(), [&](ColumnDefinition columnDef)
				{
					auto itemLength = strItemFull.GetLength();
					auto itemNextLength = vStrItems[i + 1].GetLength() - 1;
					return (totalLength + itemLength) == columnDef.Start && (itemNextLength + itemLength + totalLength) == columnDef.End;
				});
			}

			auto lastChar = strItem[strItem.GetLength() - 1];
			if (it != itWithItemNext || i == length - 1 || szWithNextItem.cx >= width || lastChar == L'\n')
			{
				CComPtr<IVariantObject> pVariantObjectItem;
				RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObjectItem));
				CComPtr<IColumnsInfoItem> pItem;
				RETURN_IF_FAILED(pChildren->AddItem(pVariantObjectItem, &pItem));
				if (it == columnDefinitions.end())
				{
					RETURN_IF_FAILED(HrCopyProp(pColumnsInfoItem, pItem, Layout::Metadata::TextColumn::Font));
					RETURN_IF_FAILED(HrCopyProp(pColumnsInfoItem, pItem, Layout::Metadata::Element::Color));
				}
				else
				{
					RETURN_IF_FAILED(HrCopyProps(it->ColumnObject, pItem));
				}

				RETURN_IF_FAILED(pItem->SetRect(itemRect));
				RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(true)));
				RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::Element::Type, &CComVar(Layout::Metadata::LayoutTypes::TextColumn)));
				RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::TextColumn::TextFullKey, &CComVar(strItemFull)));

				maxCx = max(maxCx, itemRect.right);
				maxCy = max(maxCy, itemRect.bottom);

				totalLength += strItemFull.GetLength();

				strLine.Empty();
				strItemFull.Empty();
				if (it == itWithItemNext || szWithNextItem.cx >= width)
				{
					itemRect.left = 0;
					itemRect.right = itemRect.left;
					itemRect.top = itemRect.bottom;
					width = rect.Width();
				}
				else
				{
					itemRect.left = itemRect.right;
					width = rect.Width() - itemRect.left;
				}
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