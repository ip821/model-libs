#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::PaintContainer(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
	{
		PaintRoundedRect(hdc, pColumnInfoItem);
	}

	{
		BOOL bBorderTop = FALSE;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectBoolProp(Layout::Metadata::Element::BorderTop, &bBorderTop));
		if (bBorderTop)
		{
			CComBSTR vBorderTopColor;
			RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::BorderTopColor, &vBorderTopColor));
			DWORD dwColor = 0;
			RETURN_IF_FAILED(m_pThemeColorMap->GetColor(vBorderTopColor, &dwColor));
			CBrush brush;
			brush.CreateSolidBrush(dwColor);
			CRect rect;
			RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
			CComVar vBorderTopWidth;
			RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::Element::BorderTopWidth, &vBorderTopWidth));
			ATLASSERT(vBorderTopWidth.vt == VT_I4);
			rect.bottom = rect.top + vBorderTopWidth.intVal;
			FillRect(hdc, &rect, brush);
		}
	}

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
	}
	return S_OK;
}
