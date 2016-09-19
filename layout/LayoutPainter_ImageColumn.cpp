#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::PaintImageColumn(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfoItem* pColumnInfoItem)
{
	RETURN_IF_FAILED(PaintRoundedRect(hdc, pColumnInfoItem));
	RETURN_IF_FAILED(PaintBorders(hdc, pColumnInfoItem));

	CComBSTR bstrImageKey;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::ImageColumn::ImageKey, &bstrImageKey));
	
	if(bstrImageKey == L"")
		return S_OK;

	CBitmap bitmap;
	if (FAILED(pImageManagerService->CreateImageBitmap(bstrImageKey, &bitmap.m_hBitmap)))
	{
		return S_OK;
	}

	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(hdc);
	CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);

	CRect rect;
	RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));

	CComVar vAlpha;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::Element::Alpha, &vAlpha));

	CComVar vImageStyle;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::ImageColumn::ImageStyle, &vImageStyle));

	CRect imageRect(0, 0, rect.Width(), rect.Height());
	if (vImageStyle.vt == VT_BSTR && CComBSTR(vImageStyle.bstrVal) == Layout::Metadata::ImageStyles::Fill)
	{
		if (imageRect.Width() > rect.Width())
		{
			imageRect.left = imageRect.Width() / 2 - rect.Width() / 2;
			imageRect.right = imageRect.left + rect.Width();
		}

		if (imageRect.Height() > rect.Height())
		{
			imageRect.top = imageRect.Height() / 2 - rect.Height() / 2;
			imageRect.bottom = imageRect.top + rect.Height();
		}
	}

	TBITMAP tb = { 0 };
	RETURN_IF_FAILED(pImageManagerService->GetImageInfo(bstrImageKey, &tb));

	if (tb.Width < (UINT)imageRect.Width())
	{ //sometimes desired width does not equal real image width
		imageRect.right -= imageRect.Width() - tb.Width;
	}

	if (tb.Height < (UINT)imageRect.Height())
	{ //sometimes desired height does not equal real image height
		imageRect.bottom -= imageRect.Height() - tb.Height;
	}

    if (vImageStyle.vt == VT_BSTR && CComBSTR(vImageStyle.bstrVal) == Layout::Metadata::ImageStyles::Center)
    {
        if (tb.Width > (UINT)rect.Width())
        {
            imageRect.left = tb.Width / 2 - rect.Width() / 2;
            imageRect.right = imageRect.left + rect.Width();
        }
        if (tb.Height > (UINT)rect.Height())
        {
            imageRect.top = tb.Height / 2 - rect.Height() / 2;
            imageRect.bottom = imageRect.top + rect.Height();
        }
    }

	if (vAlpha.vt == VT_UI4)
	{
		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = (BYTE)vAlpha.uintVal;
		auto res = AlphaBlend(hdc, rect.left, rect.top, rect.Width(), rect.Height(), cdcBitmap, imageRect.left, imageRect.top, imageRect.Width(), imageRect.Height(), bf);
#ifdef DEBUG
		if (!res)
			RETURN_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
#endif
	}
	else
	{
		static Gdiplus::Color color(Gdiplus::Color::Transparent);
		auto res = TransparentBlt(hdc, rect.left, rect.top, rect.Width(), rect.Height(), cdcBitmap, imageRect.left, imageRect.top, imageRect.Width(), imageRect.Height(), color.ToCOLORREF());
#ifdef DEBUG
		if (!res)
			RETURN_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
#endif
	}

	CComPtr<IColumnsInfo> pChildItems;
	RETURN_IF_FAILED(pColumnInfoItem->GetChildItems(&pChildItems));

	UINT uiCount = 0;
	RETURN_IF_FAILED(pChildItems->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IColumnsInfoItem> pOverlayItem;
		RETURN_IF_FAILED(pChildItems->GetItem(i, &pOverlayItem));
		RETURN_IF_FAILED(PaintImageColumn(hdc, pImageManagerService, pOverlayItem));
	}

	return S_OK;
}
