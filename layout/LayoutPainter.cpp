#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	return S_OK;
}

STDMETHODIMP CLayoutPainter::SetFontMap(IThemeFontMap* pThemeFontMap)
{
	CHECK_E_POINTER(pThemeFontMap);
	m_pThemeFontMap = pThemeFontMap;
	return S_OK;
}

STDMETHODIMP CLayoutPainter::EraseBackground(HDC hdc, IColumnsInfo* pColumnInfo)
{
	CHECK_E_POINTER(pColumnInfo);
	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnInfo->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnInfoItem;
		RETURN_IF_FAILED(pColumnInfo->GetItem(i, &pColumnInfoItem));
		CComBSTR bstrType;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::Type, &bstrType));
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(CLayoutBuilder::MapType(bstrType, &elementType));
		if (elementType == ElementType::HorizontalContainer || elementType == ElementType::VerticalContainer)
		{
			CRect rect;
			RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
			CComBSTR bstrBackColorName;
			RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::ColorBackground, &bstrBackColorName));
			CDCHandle cdc(hdc);
			DWORD dwColor = 0;
			RETURN_IF_FAILED(m_pThemeColorMap->GetColor(bstrBackColorName, &dwColor));
			CBrush brush;
			brush.CreateSolidBrush(dwColor);
			cdc.FillRect(rect, brush);
			break;
		}
	}
	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintLayout(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName)
{
	CPoint ptOrigin;
	RETURN_IF_FAILED(PaintLayoutInternal(hdc, pImageManagerService, pColumnInfo, bstrItemName));
	return S_OK;
}

STDMETHODIMP CLayoutPainter::PaintLayoutInternal(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName)
{
	CHECK_E_POINTER(pColumnInfo);
	UINT uiCount = 0;
	RETURN_IF_FAILED(pColumnInfo->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pColumnInfoItem;
		RETURN_IF_FAILED(pColumnInfo->GetItem(i, &pColumnInfoItem));
		BOOL bVisible = FALSE;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectBoolProp(Layout::Metadata::Element::Visible, &bVisible));
		if (!bVisible)
			continue;
		CComBSTR bstrType;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::Type, &bstrType));
		ElementType elementType = ElementType::UnknownValue;
		RETURN_IF_FAILED(CLayoutBuilder::MapType(bstrType, &elementType));
		switch (elementType)
		{
			case ElementType::HorizontalContainer:
			case ElementType::VerticalContainer:
			{
				RETURN_IF_FAILED(PaintContainer(hdc, pColumnInfoItem));
				CComPtr<IColumnsInfo> pChildItems;
				RETURN_IF_FAILED(pColumnInfoItem->GetChildItems(&pChildItems));

				CRect rect;
				RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
				RETURN_IF_FAILED(PaintLayoutInternal(hdc, pImageManagerService, pChildItems, bstrItemName));
				break;
			}
			case ElementType::TextColumn:
				RETURN_IF_FAILED(PaintTextColumn(hdc, pColumnInfoItem));
				break;
			case ElementType::ImageColumn:
				RETURN_IF_FAILED(PaintImageColumn(hdc, pImageManagerService, pColumnInfoItem));
				break;
			case ElementType::MarqueeProgressColumn:
				RETURN_IF_FAILED(PaintMarqueeProgressColumn(hdc, pColumnInfoItem));
				break;
		}
	}
	return S_OK;
}

