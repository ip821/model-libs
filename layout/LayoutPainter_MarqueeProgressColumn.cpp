#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::PaintMarqueeProgressColumn(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
	RETURN_IF_FAILED(PaintBorders(hdc, pColumnInfoItem));
	CComVar vValue;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::MarqueeProgressColumn::Value, &vValue));
	CComVar vItemSize;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::MarqueeProgressColumn::ItemSize, &vItemSize));
	CComVar vItemDistance;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::MarqueeProgressColumn::ItemDistance, &vItemDistance));
	CComVar vItemCount;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::MarqueeProgressColumn::ItemCount, &vItemCount));

	ATLASSERT(vValue.vt == VT_I4);
	ATLASSERT(vItemSize.vt == VT_I4);
	ATLASSERT(vItemDistance.vt == VT_I4);
	ATLASSERT(vItemCount.vt == VT_I4);

	auto value = vValue.intVal;
	auto itemSize = vItemSize.intVal;
	auto itemDistance = vItemDistance.intVal;
	auto itemCount = vItemCount.intVal;

	CComBSTR bstrActiveColor;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::MarqueeProgressColumn::ColorActive, &bstrActiveColor));
	CComBSTR bstrInactiveColor;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::MarqueeProgressColumn::ColorInactive, &bstrInactiveColor));

	DWORD dwActiveColor = 0;
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrActiveColor, &dwActiveColor));
	DWORD dwInactiveColor = 0;
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrInactiveColor, &dwInactiveColor));

	CBrush brushActive;
	brushActive.CreateSolidBrush(dwActiveColor);
	CBrush brushInactive;
	brushInactive.CreateSolidBrush(dwInactiveColor);

	CRect rect;
	RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
	for (int i = 0; i < itemCount; i++)
	{
		auto x = rect.left + itemSize * i + itemDistance * (max(0, i));
		auto y = rect.top;
		CRect rectItem = { (int)x, y, (int)x + itemSize, y + itemSize };
		FillRect(hdc, rectItem, i == value ? brushActive : brushInactive);
	}
	return S_OK;
}