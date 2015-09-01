#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::PaintImageColumn(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfoItem* pColumnInfoItem)
{
	RETURN_IF_FAILED(PaintRoundedRect(hdc, pColumnInfoItem));

	CComBSTR bstrImageKey;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::ImageColumn::ImageKey, &bstrImageKey));
	ATLASSERT(bstrImageKey != L"");
	CBitmap bitmap;
	if (FAILED(pImageManagerService->CreateImageBitmap(bstrImageKey, &bitmap.m_hBitmap)))
	{
		return S_OK;
	}

	TBITMAP bitmapInfo = { 0 };
	RETURN_IF_FAILED(pImageManagerService->GetImageInfo(bstrImageKey, &bitmapInfo));
	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(hdc);
	CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);

	CRect rect;
	RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));

	CComVar vAlpha;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::ImageColumn::Alpha, &vAlpha));

	CComVar vImageStyle;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::ImageColumn::ImageStyle, &vImageStyle));

	CRect imageRect(0, 0, bitmapInfo.Width, bitmapInfo.Height);
	if (vImageStyle.vt == VT_BSTR && vImageStyle.bstrVal == Layout::Metadata::ImageStyles::Fill)
	{
		imageRect.left = 0;
		imageRect.top = 0;
		imageRect.right = bitmapInfo.Width;
		imageRect.bottom = bitmapInfo.Height;

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

	if (vAlpha.vt == VT_UI4)
	{
		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = (BYTE)vAlpha.uintVal;
		auto res = AlphaBlend(hdc, rect.left, rect.top, bitmapInfo.Width, bitmapInfo.Height, cdcBitmap, imageRect.left, imageRect.top, imageRect.Width(), imageRect.Height(), bf);
		if (!res)
			return HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
		static Gdiplus::Color color(Gdiplus::Color::Transparent);
		auto res = TransparentBlt(hdc, rect.left, rect.top, bitmapInfo.Width, bitmapInfo.Height, cdcBitmap, imageRect.left, imageRect.top, imageRect.Width(), imageRect.Height(), color.ToCOLORREF());
		if (!res)
			return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
