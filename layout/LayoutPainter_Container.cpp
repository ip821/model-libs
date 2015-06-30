#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::PaintContainer(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
	BOOL bBorderBottom = FALSE;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectBoolProp(Layout::Metadata::Element::BorderBottom, &bBorderBottom));
	if (bBorderBottom)
	{
		CComBSTR vBorderBottomColor;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::BorderBottomColor, &vBorderBottomColor));
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(vBorderBottomColor, &dwColor));
		CBrush brush;
		brush.CreateSolidBrush(dwColor);
		CRect rect;
		RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
		CComVar vBorderBottomWidth;
		RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::Element::BorderBottomWidth, &vBorderBottomWidth));
		ATLASSERT(vBorderBottomWidth.vt == VT_I4);
		rect.top = rect.bottom - vBorderBottomWidth.intVal;
		FillRect(hdc, &rect, brush);
	}
	return S_OK;
}
