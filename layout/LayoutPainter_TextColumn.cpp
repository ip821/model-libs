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
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::TextColumn::TextFullKey, &bstr));

	CComBSTR bstrName;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::Name, &bstrName));

	CComBSTR bstrFont;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::TextColumn::Font, &bstrFont));

	HFONT font = 0;
	RETURN_IF_FAILED(GetItemFont(pColumnInfoItem, &font));
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

STDMETHODIMP CLayoutPainter::GetItemFont(IColumnsInfoItem* pColumnInfoItem, HFONT* pFont)
{
	CComBSTR bstrFontName = Layout::Metadata::TextColumn::Font;
	CComVar vDisabled;
	pColumnInfoItem->GetVariantValue(Layout::Metadata::Element::Disabled, &vDisabled);
	if (vDisabled.vt == VT_BOOL && vDisabled.boolVal)
	{
		bstrFontName = Layout::Metadata::TextColumn::FontDisabled;
		if (FAILED(GetFontByParamName(pColumnInfoItem, bstrFontName, pFont)))
			bstrFontName = Layout::Metadata::TextColumn::Font;
	}
	else
	{
		CComVar vSelected;
		pColumnInfoItem->GetVariantValue(Layout::Metadata::Element::Selected, &vSelected);
		if (vSelected.vt == VT_BOOL && vSelected.boolVal)
		{
			bstrFontName = Layout::Metadata::TextColumn::FontSelected;
			if (FAILED(GetFontByParamName(pColumnInfoItem, bstrFontName, pFont)))
				bstrFontName = Layout::Metadata::TextColumn::Font;
		}
	}
	RETURN_IF_FAILED(GetFontByParamName(pColumnInfoItem, bstrFontName, pFont));
	return S_OK;
}

STDMETHODIMP CLayoutPainter::GetFontByParamName(IColumnsInfoItem* pColumnInfoItem, BSTR bstrParamName, HFONT* pFont)
{
	CComVar v;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(bstrParamName, &v));
	if (v.vt != VT_BSTR)
		return E_INVALIDARG;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(v.bstrVal, pFont));
	return S_OK;
}
