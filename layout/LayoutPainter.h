#pragma once

#include "layout_i.h"
#include "Metadata.h"

using namespace ATL;

class ATL_NO_VTABLE CLayoutPainter :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLayoutPainter, &CLSID_LayoutPainter>,
	public ILayoutPainter
{
public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CLayoutPainter)
		COM_INTERFACE_ENTRY(ILayoutPainter)
	END_COM_MAP()

private:
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IThemeColorMap> m_pThemeColorMap;

	STDMETHOD(GetColorByParamName)(IColumnsInfoItem* pColumnInfoItem, BSTR bstrParamName, DWORD* pdwColor);
	STDMETHOD(GetItemBackColor)(IColumnsInfoItem* pColumnInfoItem, DWORD* pdwColor);
	STDMETHOD(GetItemColor)(IColumnsInfoItem* pColumnInfoItem, DWORD* pdwColor);
	STDMETHOD(GetItemFont)(IColumnsInfoItem* pColumnInfoItem, HFONT* pFont);
	STDMETHOD(GetFontByParamName)(IColumnsInfoItem* pColumnInfoItem, BSTR bstrParamName, HFONT* pFont);
	STDMETHOD(PaintRoundedRect)(HDC hdc, IColumnsInfoItem* pColumnInfoItem);
	STDMETHOD(PaintContainer)(HDC hdc, IColumnsInfoItem* pColumnInfoItem);
	STDMETHOD(PaintTextColumn)(HDC hdc, IColumnsInfoItem* pColumnInfoItem);
	STDMETHOD(PaintTextMultiColumn)(HDC hdc, IColumnsInfoItem* pColumnInfoItem);
	STDMETHOD(PaintImageColumn)(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfoItem* pColumnInfoItem);
	STDMETHOD(PaintMarqueeProgressColumn)(HDC hdc, IColumnsInfoItem* pColumnInfoItem);
	STDMETHOD(PaintLayoutInternal)(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName);

public:
	STDMETHOD(EraseBackground)(HDC hdc, IColumnsInfo* pColumnInfo);
	STDMETHOD(PaintLayout)(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName);

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);
};

OBJECT_ENTRY_AUTO(__uuidof(LayoutPainter), CLayoutPainter)