#include "stdafx.h"
#include "LayoutPainter.h"
#include "LayoutBuilder.h"
#include "GdilPlusUtils.h"
#include "Metadata.h"

STDMETHODIMP CLayoutPainter::PaintTextMultiColumn(HDC hdc, IColumnsInfoItem* pColumnInfoItem)
{
	RETURN_IF_FAILED(PaintContainer(hdc, pColumnInfoItem));
	return S_OK;
}