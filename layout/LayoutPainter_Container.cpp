#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::PaintContainer(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
	RETURN_IF_FAILED(PaintRoundedRect(hdc, pColumnInfoItem));
	RETURN_IF_FAILED(PaintBorders(hdc, pColumnInfoItem));

	return S_OK;
}
