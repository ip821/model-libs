#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::PaintTextColumn(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
	CDCHandle cdc(hdc);

	CRect rect;
	RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));

	CComBSTR bstrBackStyle;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::BackgroundStyle, &bstrBackStyle));

	if (bstrBackStyle != L"" && bstrBackStyle == Layout::Metadata::BackgroundStyles::Rounded)
	{
		CComBSTR bstrBackColorName;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::ColorBackground, &bstrBackColorName));
		DWORD dwColor = 0;
		RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrBackColorName, &dwColor));
		DrawRoundedRect(cdc, rect, false, dwColor);
	}

	CComBSTR bstr;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::TextColumn::Text, &bstr));

	CComBSTR bstrName;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::Name, &bstrName));

	CComBSTR bstrFont;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::TextColumn::Font, &bstrFont));

	HFONT font = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(bstrFont, &font));
	CDCSelectFontScope cdcSelectFontScope(cdc, font);

	BOOL bSelected = FALSE;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectBoolProp(Layout::Metadata::Element::Selected, &bSelected));

	CComBSTR bstrColor;
	DWORD dwColor = 0;
	if (bSelected)
	{
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::TextColumn::ColorSelected, &bstrColor));
	}
	else
	{
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::TextColumn::Color, &bstrColor));
	}
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrColor, &dwColor));

	cdc.SetBkMode(TRANSPARENT);
	cdc.SetTextColor(dwColor);

	DrawText(cdc, bstr, bstr.Length(), &rect, 0);

	return S_OK;
}
