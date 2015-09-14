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
			CDCHandle cdc(hdc);
			DWORD dwColor = 0;
			RETURN_IF_FAILED(GetItemBackColor(pColumnInfoItem, &dwColor));
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

class AlphaPaintScope
{
private:
	CDCHandle m_hdc;
	CComPtr<IColumnsInfoItem> m_pItem;
	const DWORD MAX_ALPHA = 255;
	CDC m_cdc;
	shared_ptr<CBitmap> m_pBitmap;
	CRect m_rect;
	DWORD m_dwAlpha = MAX_ALPHA;

public:
	AlphaPaintScope(HDC hdc, IColumnsInfoItem* pItem)
	{
		m_hdc = hdc;
		m_pItem = pItem;
		pItem->GetRect(&m_rect);
		CComVar vAlpha;
		pItem->GetVariantValue(Layout::Metadata::Element::Alpha, &vAlpha);
		if (vAlpha.vt == VT_UI4 && vAlpha.uintVal != MAX_ALPHA)
		{
			m_dwAlpha = vAlpha.uintVal;
			m_cdc.CreateCompatibleDC(m_hdc);
			m_pBitmap = make_shared<CBitmap>();
			m_pBitmap->CreateCompatibleBitmap(m_hdc, m_rect.Width(), m_rect.Height());
			m_cdc.SelectBitmap(m_pBitmap.get()->m_hBitmap);
		}
	}

	HDC& GetHdc()
	{
		if (m_cdc.m_hDC)
			return m_cdc.m_hDC;
		return m_hdc.m_hDC;
	}

	void Flush()
	{
		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = (BYTE)m_dwAlpha;
		m_hdc.AlphaBlend(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_cdc, 0, 0, m_rect.Width(), m_rect.Height(), bf);
	}
};

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
				{
					AlphaPaintScope aps(hdc, pColumnInfoItem);
					RETURN_IF_FAILED(PaintContainer(aps.GetHdc(), pColumnInfoItem));
					CComPtr<IColumnsInfo> pChildItems;
					RETURN_IF_FAILED(pColumnInfoItem->GetChildItems(&pChildItems));

					CRect rect;
					RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));
					RETURN_IF_FAILED(PaintLayoutInternal(aps.GetHdc(), pImageManagerService, pChildItems, bstrItemName));
					aps.Flush();
				}
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

STDMETHODIMP CLayoutPainter::PaintRoundedRect(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
#ifdef DEBUG
	CComBSTR bstrNameDebug;
	pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::Name, &bstrNameDebug);
	if (bstrNameDebug == L"TwitterUserImageContainer")
	{
		CString str = bstrNameDebug;
	}
#endif
	CRect rect;
	RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));

	{
		CComBSTR bstrBackStyle;
		RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::Element::BackgroundStyle, &bstrBackStyle));

		CString strBackStyle(bstrBackStyle);
		if (bstrBackStyle != L"" && strBackStyle.Find(Layout::Metadata::BackgroundStyles::Rounded) != -1)
		{
			DWORD dwColor = 0;
			RETURN_IF_FAILED(GetItemBackColor(pColumnInfoItem, &dwColor));
			CDCHandle cdc(hdc);
			DrawRoundedRect(cdc, rect, strBackStyle == Layout::Metadata::BackgroundStyles::RoundedStrict ? true : false, dwColor);
		}
	}

	return S_OK;
}

STDMETHODIMP CLayoutPainter::GetColorByParamName(IColumnsInfoItem* pColumnInfoItem, BSTR bstrParamName, DWORD* pdwColor)
{
	CComVar v;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(bstrParamName, &v));
	if (v.vt != VT_BSTR)
		return E_INVALIDARG;
	RETURN_IF_FAILED(m_pThemeColorMap->GetColor(v.bstrVal, pdwColor));
	return S_OK;
}

STDMETHODIMP CLayoutPainter::GetItemBackColor(IColumnsInfoItem* pColumnInfoItem, DWORD* pdwColor)
{
	CComBSTR bstrColorName = Layout::Metadata::Element::ColorBackground;
	CComVar vDisabled;
	pColumnInfoItem->GetVariantValue(Layout::Metadata::Element::Disabled, &vDisabled);
	if (vDisabled.vt == VT_BOOL && vDisabled.boolVal)
	{
		bstrColorName = Layout::Metadata::Element::ColorBackgroundDisabled;
		if (FAILED(GetColorByParamName(pColumnInfoItem, bstrColorName, pdwColor)))
			bstrColorName = Layout::Metadata::Element::ColorBackground;
	}
	else
	{
		CComVar vSelected;
		pColumnInfoItem->GetVariantValue(Layout::Metadata::Element::Selected, &vSelected);
		if (vSelected.vt == VT_BOOL && vSelected.boolVal)
		{
			bstrColorName = Layout::Metadata::Element::ColorBackgroundSelected;
			if (FAILED(GetColorByParamName(pColumnInfoItem, bstrColorName, pdwColor)))
				bstrColorName = Layout::Metadata::Element::ColorBackground;
		}
	}
	RETURN_IF_FAILED(GetColorByParamName(pColumnInfoItem, bstrColorName, pdwColor));
	return S_OK;
}

STDMETHODIMP CLayoutPainter::GetItemColor(IColumnsInfoItem* pColumnInfoItem, DWORD* pdwColor)
{
	CComBSTR bstrColorName = Layout::Metadata::Element::Color;
	CComVar vDisabled;
	pColumnInfoItem->GetVariantValue(Layout::Metadata::Element::Disabled, &vDisabled);
	if (vDisabled.vt == VT_BOOL && vDisabled.boolVal)
	{
		bstrColorName = Layout::Metadata::Element::ColorDisabled;
		if (FAILED(GetColorByParamName(pColumnInfoItem, bstrColorName, pdwColor)))
			bstrColorName = Layout::Metadata::Element::Color;
	}
	else
	{
		CComVar vSelected;
		pColumnInfoItem->GetVariantValue(Layout::Metadata::Element::Selected, &vSelected);
		if (vSelected.vt == VT_BOOL && vSelected.boolVal)
		{
			bstrColorName = Layout::Metadata::Element::ColorSelected;
			if (FAILED(GetColorByParamName(pColumnInfoItem, bstrColorName, pdwColor)))
				bstrColorName = Layout::Metadata::Element::Color;
		}
	}
	RETURN_IF_FAILED(GetColorByParamName(pColumnInfoItem, bstrColorName, pdwColor));
	return S_OK;
}
