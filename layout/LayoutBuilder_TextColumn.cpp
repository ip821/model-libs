#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"

STDMETHODIMP CLayoutBuilder::BuildTextColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	CComVar vTextKey;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextColumn::Text, &vTextKey));
	ATLASSERT(vTextKey.vt == VT_BSTR);

	CComVar vFont;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextColumn::Font, &vFont));
	ATLASSERT(vFont.vt == VT_BSTR);

	HFONT font = 0;
	RETURN_IF_FAILED(m_pThemeFontMap->GetFont(vFont.bstrVal, &font));
	CDCSelectFontScope cdcSelectFontScope(hdc, font);

	CComBSTR bstr = vTextKey.bstrVal;
	CSize sz;
	GetTextExtentPoint32(hdc, bstr, bstr.Length(), &sz);

	CRect sourceRect = *pSourceRect;
	CRect textRect;
	textRect.left = sourceRect.left;
	textRect.top = sourceRect.top;
	textRect.right = textRect.left + sz.cx;
	textRect.bottom = textRect.top + sz.cy;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->QueryInterface(ppColumnsInfoItem));
	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(textRect));

	CComVar vColor;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextColumn::Color, &vColor));
	ATLASSERT(vColor.vt == VT_BSTR);
	CComVar vColorSelected;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextColumn::ColorSelected, &vColorSelected));
	CComVar vText;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextColumn::Text, &vText));
	ATLASSERT(vText.vt == VT_BSTR);

	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Layout::Metadata::TextColumn::Color, vColor.bstrVal));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Layout::Metadata::TextColumn::Text, vText.bstrVal));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Layout::Metadata::TextColumn::Font, vFont.bstrVal));

	if (vColorSelected.vt == VT_BSTR)
	{
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Layout::Metadata::TextColumn::ColorSelected, vColorSelected.bstrVal));
	}

	*pDestRect = textRect;
	return S_OK;
}
