#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "..\objmdl\StringUtils.h"
#include "LayoutPainter.h"

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
		vector<ItemDescriptor> vStrItems;

		vector<ColumnDefinition> columnDefinitions;

		CComVar vColumnDefinition;
		if (pValueObject)
		{
			RETURN_IF_FAILED(pValueObject->GetVariantValue(Layout::Metadata::TextMultiColumn::ColumnDefinitions, &vColumnDefinition));
		}

		if (vColumnDefinition.vt == VT_EMPTY)
		{
			RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextMultiColumn::ColumnDefinitions, &vColumnDefinition));
		}

		if (vColumnDefinition.vt != VT_EMPTY)
		{
			ATLASSERT(vColumnDefinition.vt == VT_UNKNOWN);
			CComQIPtr<IObjCollection> pObjCollection = vColumnDefinition.punkVal;

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
				ATLASSERT(vStartIndex.vt == VT_I4);
				ATLASSERT(vEndIndex.vt == VT_I4);
				ATLASSERT(vColumnDef.vt == VT_UNKNOWN);
				ColumnDefinition cd;
				cd.Start = vStartIndex.intVal;
				cd.End = vEndIndex.intVal;
				cd.ColumnObject = vColumnDef.punkVal;
				columnDefinitions.push_back(cd);
			}
		}

		HFONT font = 0;
		RETURN_IF_FAILED(CLayoutPainter::GetItemFont(m_pThemeFontMap, pColumnsInfoItem, &font));
		CDCSelectFontScope cdcSelectFontScope(hdc, font);

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
				ItemDescriptor itemDescriptor;
				itemDescriptor.Text = item;
				if (it != columnDefinitions.end())
					itemDescriptor.pColumnDefinition = &(*it);

				GetTextExtentPoint32(hdc, item, item.GetLength(), &itemDescriptor.Size);
				vStrItems.push_back(itemDescriptor);
				item.Empty();
			}
		}

		CComPtr<IColumnsInfo> pChildren;
		RETURN_IF_FAILED(pColumnsInfoItem->GetChildItems(&pChildren));

		CString strLine;
		auto size = vStrItems.size();
		CRect rect = *pSourceRect;
		auto width = rect.Width();
		CRect itemRect;
		int maxCx = 0;
		int maxCy = 0;
		for (size_t i = 0; i < size; i++)
		{
			ItemDescriptor& item = vStrItems[i];
			strLine += item.Text;

			itemRect.right += item.Size.cx;
			itemRect.bottom = max(itemRect.top + item.Size.cy, itemRect.bottom);

			if (i == size - 1 || item.pColumnDefinition != vStrItems[i + 1].pColumnDefinition || (itemRect.Width() + vStrItems[i + 1].Size.cx) > width || strLine[strLine.GetLength() - 1] == L'\n')
			{
				CComPtr<IVariantObject> pVariantObjectItem;
				RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObjectItem));

				CComPtr<IColumnsInfoItem> pItem;
				RETURN_IF_FAILED(pChildren->AddItem(pVariantObjectItem, &pItem));

				if (item.pColumnDefinition)
				{
					RETURN_IF_FAILED(HrCopyProps(item.pColumnDefinition->ColumnObject, pItem));
				}
				else
				{
					RETURN_IF_FAILED(HrCopyProp(pColumnsInfoItem, pItem, Layout::Metadata::TextColumn::Font));
					RETURN_IF_FAILED(HrCopyProp(pColumnsInfoItem, pItem, Layout::Metadata::Element::Color));
				}

				RETURN_IF_FAILED(pItem->SetRect(itemRect));
				RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(true)));
				RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::Element::Type, &CComVar(Layout::Metadata::LayoutTypes::TextColumn)));
				RETURN_IF_FAILED(pItem->SetVariantValue(Layout::Metadata::TextColumn::TextFullKey, &CComVar(strLine)));

				maxCx = max(maxCx, itemRect.right);
				maxCy = max(maxCy, itemRect.bottom);

				if (i != size - 1)
				{
					if (item.pColumnDefinition == vStrItems[i + 1].pColumnDefinition || (itemRect.Width() + vStrItems[i + 1].Size.cx) > width || strLine[strLine.GetLength() - 1] == L'\n')
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

				strLine.Empty();
			}
		}

		CRect rectResultRect = *pSourceRect;
		rectResultRect.right = rectResultRect.left + maxCx;
		rectResultRect.bottom = rectResultRect.top + maxCy;
		RETURN_IF_FAILED(pColumnsInfoItem->SetVariantValue(Layout::Metadata::Element::Visible, &CComVar(true)));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectResultRect));
		*pDestRect = rectResultRect;
		RETURN_IF_FAILED(pColumnsInfoItem->QueryInterface(ppColumnsInfoItem));
	}

	return S_OK;
}