#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"

STDMETHODIMP CLayoutBuilder::BuildImageColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	CComVar vImageKey;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::ImageColumn::ImageKey, &vImageKey));
	ATLASSERT(vImageKey.vt == VT_BSTR);
	TBITMAP bitmapInfo = { 0 };
	RETURN_IF_FAILED(pImageManagerService->GetImageInfo(vImageKey.bstrVal, &bitmapInfo));

	CRect sourceRect = *pSourceRect;
	CRect imageRect;
	imageRect.left = sourceRect.left;
	imageRect.top = sourceRect.top;
	imageRect.right = imageRect.left + bitmapInfo.Width;
	imageRect.bottom = imageRect.top + bitmapInfo.Height;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->QueryInterface(ppColumnsInfoItem));
	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRect(imageRect));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Layout::Metadata::ImageColumn::ImageKey, vImageKey.bstrVal));
	*pDestRect = imageRect;
	return S_OK;
}
