#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "LayoutPainter.h"

STDMETHODIMP CLayoutBuilder::VarToString(CComVar& v, CComBSTR& bstr)
{
	switch (v.vt)
	{
		case VT_BSTR:
			bstr = v.bstrVal;
			break;
		case VT_I4:
			bstr = boost::lexical_cast<wstring>(v.intVal).c_str();
			break;
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::GetTextForTextColumn(IVariantObject* pLayoutObject, IVariantObject* pValueObject, BSTR* pbstrText)
{
	CHECK_E_POINTER(pLayoutObject);
	CHECK_E_POINTER(pbstrText);
	CComBSTR bstrText;

	CComVar vText;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextColumn::Text, &vText));
	VarToString(vText, bstrText);

	if (pValueObject)
	{
		CComVar vTextKey;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextColumn::TextKey, &vTextKey));
		if (vTextKey.vt == VT_BSTR)
		{
			CComVar vTextByKey;
			RETURN_IF_FAILED(pValueObject->GetVariantValue(vTextKey.bstrVal, &vTextByKey));
			CComBSTR bstrTextByKey;
			VarToString(vTextByKey, bstrTextByKey);
			bstrText += bstrTextByKey;
		}
	}
	*pbstrText = bstrText.Detach();
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildTextColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	CComVar vVisible;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::Element::Visible, &vVisible));
	ATLASSERT(vVisible.vt == VT_BOOL);

	CRect textRect;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(pLayoutObject, &pColumnsInfoItem));

	if (vVisible.boolVal)
	{
		CComBSTR bstrText;
		RETURN_IF_FAILED(GetTextForTextColumn(pLayoutObject, pValueObject, &bstrText));
		RETURN_IF_FAILED(pColumnsInfoItem->SetVariantValue(Layout::Metadata::TextColumn::TextFullKey, &CComVar(bstrText)));

		HFONT font = 0;
		RETURN_IF_FAILED(CLayoutPainter::GetItemFont(m_pThemeFontMap, pColumnsInfoItem, &font));
		CDCSelectFontScope cdcSelectFontScope(hdc, font);

		CComVar vMultiline;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::TextColumn::Multiline, &vMultiline));
		BOOL bWordWrap = vMultiline.vt == VT_BOOL && vMultiline.boolVal;

		CSize sz;
		if (bWordWrap)
		{
			CRect rectSource = *pSourceRect;
			CRect rect;
			rect.right = rectSource.Width();
			rect.bottom = rectSource.Height();

			if (bstrText != L"")
			{
				DrawText(hdc, bstrText, bstrText.Length(), &rect, DT_WORDBREAK | DT_CALCRECT);
				sz.cx = rect.Width();
				sz.cy = rect.Height();
			}
		}
		else
		{
			GetTextExtentPoint32(hdc, bstrText, bstrText.Length(), &sz);
		}

		CRect sourceRect = *pSourceRect;
		textRect.left = sourceRect.left;
		textRect.top = sourceRect.top;
		textRect.right = textRect.left + sz.cx;
		textRect.bottom = textRect.top + sz.cy;

		RETURN_IF_FAILED(pColumnsInfoItem->QueryInterface(ppColumnsInfoItem));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(textRect));

		*pDestRect = textRect;
	}
	return S_OK;
}
