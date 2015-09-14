#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"

STDMETHODIMP CLayoutBuilder::BuildMarqueeProgressColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	CComVar vVisible;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::Element::Visible, &vVisible));
	ATLASSERT(vVisible.vt == VT_BOOL);

	CRect columnRect;

	if (vVisible.boolVal)
	{
		CComVar vItemSize;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::MarqueeProgressColumn::ItemSize, &vItemSize));
		CComVar vItemDistance;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::MarqueeProgressColumn::ItemDistance, &vItemDistance));
		CComVar vItemCount;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::MarqueeProgressColumn::ItemCount, &vItemCount));

		ATLASSERT(vItemSize.vt == VT_I4);
		ATLASSERT(vItemDistance.vt == VT_I4);
		ATLASSERT(vItemCount.vt == VT_I4);

		auto itemSize = vItemSize.intVal;
		auto itemDistance = vItemDistance.intVal;
		auto itemCount = vItemCount.intVal;

		CRect sourceRect = *pSourceRect;
		columnRect.left = sourceRect.left;
		columnRect.top = sourceRect.top;

		int width = itemSize * itemCount + itemDistance * (itemCount - 1);
		columnRect.right = columnRect.left + width;
		columnRect.bottom = columnRect.top + itemSize;

		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnInfo->AddItem(pLayoutObject, &pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->QueryInterface(ppColumnsInfoItem));
		RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(columnRect));
	}
	*pDestRect = columnRect;
	return S_OK;
}
