#include "stdafx.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"

STDMETHODIMP CLayoutBuilder::BuildImageColumn(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem)
{
	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	RETURN_IF_FAILED(pColumnInfo->AddItem(&pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->QueryInterface(ppColumnsInfoItem));
	RETURN_IF_FAILED(SetColumnProps(pLayoutObject, pColumnsInfoItem));

	CComVar vVisible;
	RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::Element::Visible, &vVisible));
	ATLASSERT(vVisible.vt == VT_BOOL);

	CRect imageRect;

	if (vVisible.boolVal)
	{
		CComVar vHeight;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::ImageColumn::Height, &vHeight));
		CComVar vWidth;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::ImageColumn::Width, &vWidth));

		CComVar vImageKey;
		RETURN_IF_FAILED(pLayoutObject->GetVariantValue(Layout::Metadata::ImageColumn::ImageKey, &vImageKey));
		ATLASSERT(vImageKey.vt == VT_BSTR);

		int height = 0;
		int width = 0;

		if (vHeight.vt == VT_I4)
			height = vHeight.intVal;

		if (vWidth.vt == VT_I4)
			width = vWidth.intVal;

		if (!height || !width)
		{
			TBITMAP bitmapInfo = { 0 };
			RETURN_IF_FAILED(pImageManagerService->GetImageInfo(vImageKey.bstrVal, &bitmapInfo));
			if (!height)
				height = bitmapInfo.Height;
			if (!width)
				width = bitmapInfo.Width;
		}

		imageRect.left = pSourceRect->left;
		imageRect.top = pSourceRect->top;
		imageRect.right = imageRect.left + width;
		imageRect.bottom = imageRect.top + height;

		RETURN_IF_FAILED(pColumnsInfoItem->SetRect(imageRect));
		RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Layout::Metadata::ImageColumn::ImageKey, vImageKey.bstrVal));
	}

	*pDestRect = imageRect;
	return S_OK;
}
