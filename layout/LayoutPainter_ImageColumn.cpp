#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::PaintImageColumn(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfoItem* pColumnInfoItem)
{
	CComBSTR bstrImageKey;
	RETURN_IF_FAILED(pColumnInfoItem->GetRectStringProp(Layout::Metadata::ImageColumn::ImageKey, &bstrImageKey));
	ATLASSERT(bstrImageKey != L"");
	CBitmap bitmap;
	RETURN_IF_FAILED(pImageManagerService->CreateImageBitmap(bstrImageKey, &bitmap.m_hBitmap));

	TBITMAP bitmapInfo = { 0 };
	RETURN_IF_FAILED(pImageManagerService->GetImageInfo(bstrImageKey, &bitmapInfo));
	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(hdc);
	CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);

	CRect rect;
	RETURN_IF_FAILED(pColumnInfoItem->GetRect(&rect));

	CComVar vAlpha;
	RETURN_IF_FAILED(pColumnInfoItem->GetVariantValue(Layout::Metadata::ImageColumn::Alpha, &vAlpha));

	if (vAlpha.vt == VT_UI4)
	{
		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = (BYTE)vAlpha.uintVal;
		auto res = AlphaBlend(hdc, rect.left, rect.top, bitmapInfo.Width, bitmapInfo.Height, cdcBitmap, 0, 0, bitmapInfo.Width, bitmapInfo.Height, bf);
		if (!res)
			return HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
		static Gdiplus::Color color(Gdiplus::Color::Transparent);
		auto res = TransparentBlt(hdc, rect.left, rect.top, bitmapInfo.Width, bitmapInfo.Height, cdcBitmap, 0, 0, bitmapInfo.Width, bitmapInfo.Height, color.ToCOLORREF());
		if (!res)
			return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
