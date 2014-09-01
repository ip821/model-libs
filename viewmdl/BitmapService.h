// BitmapService.h : Declaration of the CBitmapService

#pragma once
#include "resource.h"       // main symbols
#include "ViewMdl_i.h"

using namespace ATL;


// CBitmapService

class ATL_NO_VTABLE CBitmapService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBitmapService, &CLSID_BitmapService>,
	public IBitmapService
{
public:
	CBitmapService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CBitmapService)
		COM_INTERFACE_ENTRY(IBitmapService)
	END_COM_MAP()

public:

	STDMETHOD(MakeBitMapTransparent)(HBITMAP hbmSrc, HBITMAP* phbmDest)
	{
		CHECK_E_POINTER(phbmDest);
		HDC hdcSrc = 0, hdcDst = 0;
		HBITMAP hbmOld = 0, hbmNew = 0;
		BITMAP bm = {0};
		COLORREF clrTP = 0, clrBK = 0;

		if ((hdcSrc = CreateCompatibleDC(NULL)) == NULL)
			return E_FAIL;

		if ((hdcDst = CreateCompatibleDC(NULL)) == NULL)
			return E_FAIL;

		int nRow, nCol;
		GetObject(hbmSrc, sizeof(bm), &bm);
		hbmOld = (HBITMAP)SelectObject(hdcSrc, hbmSrc);
		hbmNew = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL);
		SelectObject(hdcDst, hbmNew);
		BitBlt(hdcDst,0,0,bm.bmWidth, bm.bmHeight,hdcSrc,0,0,SRCCOPY);
		clrTP = GetPixel(hdcDst, 0, 0);
		clrBK = GetSysColor(COLOR_MENU);

		for (nRow = 0; nRow < bm.bmHeight; nRow++)
		{
			for (nCol = 0; nCol < bm.bmWidth; nCol++)// when we hit our set transparency color.
			{
				if (GetPixel(hdcDst, nCol, nRow) == clrTP)
					SetPixel(hdcDst, nCol, nRow, clrBK);
			}
		}
		DeleteDC(hdcDst);
		DeleteDC(hdcSrc);

		*phbmDest = hbmNew;
		return S_OK;
	}

};

OBJECT_ENTRY_AUTO(__uuidof(BitmapService), CBitmapService)
