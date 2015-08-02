#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"

STDMETHODIMP CLayoutBuilder::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::BuildLayout(HDC hdc, RECT* pSourceRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo)
{
	CRect rect;
	CComPtr<IColumnsInfoItem> pColumnsInfoItem;

	ElementType elementType = ElementType::UnknownValue;
	RETURN_IF_FAILED(GetElementType(pLayoutObject, &elementType));

	switch (elementType)
	{
		case ElementType::HorizontalContainer:
			RETURN_IF_FAILED(BuildHorizontalContainer(hdc, pSourceRect, &rect, pLayoutObject, pValueObject, pImageManagerService, pColumnInfo, &pColumnsInfoItem));
			break;

		case ElementType::VerticalContainer:
			RETURN_IF_FAILED(BuildVerticalContainer(hdc, pSourceRect, &rect, pLayoutObject, pValueObject, pImageManagerService, pColumnInfo, &pColumnsInfoItem));
			break;
	}

	CPoint pt;
	RETURN_IF_FAILED(TranslateRects(&pt, pColumnInfo));
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::TranslateRects(POINT* ptOrigin, IColumnsInfo* pColumnsInfo)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnsInfo->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pColumnsInfo->GetItem(i, &pColumnsInfoItem));
		CRect rect;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&rect));
		rect.OffsetRect(*ptOrigin);
		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rect));
		CComPtr<IColumnsInfo> pChildren;
		RETURN_IF_FAILED(pColumnsInfoItem->GetChildItems(&pChildren));
		CPoint pt(rect.left, rect.top);
		RETURN_IF_FAILED(TranslateRects(&pt, pChildren));
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::GetElements(IVariantObject* pVariantObject, IObjArray** ppObjArray)
{
	CHECK_E_POINTER(pVariantObject);
	CHECK_E_POINTER(ppObjArray);
	CComVar vElements;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Layout::Metadata::Element::Elements, &vElements));
	ATLASSERT(vElements.vt == VT_UNKNOWN);
	RETURN_IF_FAILED(vElements.punkVal->QueryInterface(ppObjArray));
	return S_OK;
}

HRESULT CLayoutBuilder::MapType(BSTR bstrType, ElementType* pElementType)
{
	CComBSTR type(bstrType);
	if (type == CComBSTR(Layout::Metadata::LayoutTypes::HorizontalContainer))
		*pElementType = ElementType::HorizontalContainer;
	else if (type == CComBSTR(Layout::Metadata::LayoutTypes::VerticalContainer))
		*pElementType = ElementType::VerticalContainer;
	else if (type == CComBSTR(Layout::Metadata::LayoutTypes::ImageColumn))
		*pElementType = ElementType::ImageColumn;
	else if (type == CComBSTR(Layout::Metadata::LayoutTypes::TextColumn))
		*pElementType = ElementType::TextColumn;
	else if (type == CComBSTR(Layout::Metadata::LayoutTypes::MarqueeProgressColumn))
		*pElementType = ElementType::MarqueeProgressColumn;
	else
		*pElementType = ElementType::UnknownValue;
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::GetElementType(IVariantObject* pVariantObject, ElementType* pElementType)
{
	CHECK_E_POINTER(pElementType);
	CComVar vLayoutType;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Layout::Metadata::Element::Type, &vLayoutType));
	ATLASSERT(vLayoutType.vt == VT_BSTR);
	RETURN_IF_FAILED(MapType(vLayoutType.bstrVal, pElementType));
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::ApplyAlignVertical(IColumnsInfo* pChildItems, CRect& rectParent, CRect& rect)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pChildItems->GetCount(&uiCount));
	int maxBottom = rectParent.bottom;
	while (uiCount > 0)
	{
		uiCount--;
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pChildItems->GetItem(uiCount, &pColumnsInfoItem));
		CRect rectChild;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&rectChild));
		CComBSTR bstrAlign;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Layout::Metadata::Element::AlignVertical, &bstrAlign));
		if (bstrAlign == Layout::Metadata::AlignVerticalTypes::Down)
		{
			{
				auto rectChildWithMargins = rectChild;
				RETURN_IF_FAILED(ApplyEndMargins(pColumnsInfoItem, rectChildWithMargins));
				auto diff = rectChildWithMargins.bottom - rectChild.bottom;
				maxBottom -= diff;
			}

			rectChild.top = maxBottom - rectChild.Height();
			rectChild.bottom = maxBottom;
			maxBottom -= rectChild.Height();

			{
				auto rectChildWithMargins = rectChild;
				RETURN_IF_FAILED(ApplyStartMargins(pColumnsInfoItem, rectChildWithMargins));
				auto diff = rectChildWithMargins.top - rectChild.top;
				maxBottom -= diff;
			}

			RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectChild));
		}
		else if (bstrAlign == Layout::Metadata::AlignVerticalTypes::Center)
		{
			auto y = rectParent.Height() / 2 - rectChild.Height() / 2;
			auto diff = y - rectChild.top;
			rectChild.top += diff;
			rectChild.bottom += diff;
			RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectChild));
		}
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::ApplyAlignHorizontal(IColumnsInfo* pChildItems, CRect& rectParent, CRect& rect)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pChildItems->GetCount(&uiCount));
	int maxRight = rectParent.right;
	while (uiCount > 0)
	{
		uiCount--;
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		RETURN_IF_FAILED(pChildItems->GetItem(uiCount, &pColumnsInfoItem));
		CRect rectChild;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRect(&rectChild));
		CComBSTR bstrAlign;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Layout::Metadata::Element::AlignHorizontal, &bstrAlign));
		if (bstrAlign == Layout::Metadata::AlignHorizontalTypes::Right)
		{
			{
				auto rectChildWithMargins = rectChild;
				RETURN_IF_FAILED(ApplyEndMargins(pColumnsInfoItem, rectChildWithMargins));
				auto diff = rectChildWithMargins.right - rectChild.right;
				maxRight -= diff;
			}

			rectChild.left = maxRight - rectChild.Width();
			rectChild.right = maxRight;
			maxRight -= rectChild.Width();

			{
				auto rectChildWithMargins = rectChild;
				RETURN_IF_FAILED(ApplyStartMargins(pColumnsInfoItem, rectChildWithMargins));
				auto diff = rectChildWithMargins.left - rectChild.left;
				maxRight -= diff;
			}
			CComPtr<IColumnsInfo> pChildrenOfChild;
			RETURN_IF_FAILED(pColumnsInfoItem->GetChildItems(&pChildrenOfChild));
			RETURN_IF_FAILED(ApplyAlignHorizontal(pChildrenOfChild, rectChild, rect));
			RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectChild));
		}
		else if (bstrAlign == Layout::Metadata::AlignHorizontalTypes::Center)
		{
			auto x = rectParent.Width() / 2 - rectChild.Width() / 2;
			auto diff = x - rectChild.left;
			rectChild.left += diff;
			rectChild.right += diff;
			RETURN_IF_FAILED(pColumnsInfoItem->SetRect(rectChild));
		}
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::FitToParent(IVariantObject* pElement, CRect& rectParent, CRect& rect)
{
	CComVar vFitHorizantal;
	CComVar vFitVertical;
	pElement->GetVariantValue(Layout::Metadata::Element::FitHorizontal, &vFitHorizantal);
	pElement->GetVariantValue(Layout::Metadata::Element::FitVertical, &vFitVertical);

	if (vFitHorizantal.vt == VT_BSTR && CComBSTR(vFitHorizantal.bstrVal) == Layout::Metadata::FitTypes::Parent)
	{
		rect.right = rectParent.right;
	}

	if (vFitVertical.vt == VT_BSTR && CComBSTR(vFitVertical.bstrVal) == Layout::Metadata::FitTypes::Parent)
	{
		rect.bottom = rectParent.bottom;
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::ApplyEndMargins(IVariantObject* pElement, CRect& rect)
{
	CComVar vMarginRight;
	CComVar vMarginBottom;
	pElement->GetVariantValue(Layout::Metadata::Element::MarginRight, &vMarginRight);
	pElement->GetVariantValue(Layout::Metadata::Element::MarginBottom, &vMarginBottom);

	if (vMarginRight.vt == VT_I4)
	{
		auto val = vMarginRight.intVal;
		rect.right += val;
	}

	if (vMarginBottom.vt == VT_I4)
	{
		auto val = vMarginBottom.intVal;
		rect.bottom += val;
	}
	return S_OK;
}

STDMETHODIMP CLayoutBuilder::ApplyStartMargins(IVariantObject* pElement, CRect& rect)
{
	CHECK_E_POINTER(pElement);

	CComVar vMarginLeft;
	CComVar vMarginTop;

	pElement->GetVariantValue(Layout::Metadata::Element::MarginLeft, &vMarginLeft);
	pElement->GetVariantValue(Layout::Metadata::Element::MarginTop, &vMarginTop);

	if (vMarginLeft.vt == VT_I4)
	{
		auto val = vMarginLeft.intVal;
		auto width = rect.Width();
		rect.left += val;
		rect.right = rect.left + width;
	}

	if (vMarginTop.vt == VT_I4)
	{
		auto val = vMarginTop.intVal;
		auto height = rect.Height();
		rect.top += val;
		rect.bottom = rect.top + height;
	}

	return S_OK;
}

STDMETHODIMP CLayoutBuilder::SetColumnProps(IVariantObject* pLayoutObject, IColumnsInfoItem* pColumnsInfoItem)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pLayoutObject->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComBSTR bstrKey;
		RETURN_IF_FAILED(pLayoutObject->GetKeyByIndex(i, &bstrKey));
		CComVar vValue;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(bstrKey, &vValue));
		RETURN_IF_FAILED(pColumnsInfoItem->SetVariantValue(bstrKey, &vValue));
	}

	CComVar vName;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::Element::Name, &vName));
	ATLASSERT(vName.vt == VT_BSTR);
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Layout::Metadata::Column::Name, vName.bstrVal));
	return S_OK;
}
