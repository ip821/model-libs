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
	RETURN_IF_FAILED(PaintRoundedRect(hdc, pColumnInfoItem));

	CComBSTR bstr;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::TextColumn::Text, &bstr));

	CComBSTR bstrName;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::Name, &bstrName));

	CComBSTR bstrFont;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::TextColumn::Font, &bstrFont));

	HFONT font = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(bstrFont, &font));
	CDCSelectFontScope cdcSelectFontScope(cdc, font);

	CComBSTR bstrColor;
	DWORD dwColor = 0;
	RETURN_IF_FAILED(GetItemColor(pColumnInfoItem, &dwColor));

	cdc.SetBkMode(TRANSPARENT);
	cdc.SetTextColor(dwColor);

	CComVar vMultiline;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::TextColumn::Multiline, &vMultiline));
	BOOL bWordWrap = vMultiline.vt == VT_BOOL && vMultiline.boolVal;

	DrawText(cdc, bstr, bstr.Length(), &rect, bWordWrap ? DT_WORDBREAK : 0);

	return S_OK;
}
