#pragma once

#include "layout_i.h"
#include "ElementType.h"
#include "Metadata.h"

using namespace ATL;

class ATL_NO_VTABLE CLayoutBuilder :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLayoutBuilder, &CLSID_LayoutBuilder>,
	public ILayoutBuilder
{
public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CLayoutBuilder)
		COM_INTERFACE_ENTRY(ILayoutBuilder)
	END_COM_MAP()

private:
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IThemeColorMap> m_pThemeColorMap;

	STDMETHOD(GetElements)(IVariantObject* pVariantObject, IObjArray** ppObjArray);
	STDMETHOD(ApplyStartMargins)(IVariantObject* pElement, CRect& rect);
	STDMETHOD(ApplyEndMargins)(IVariantObject* pElement, CRect& rect);
	STDMETHOD(FitToParentStart)(IVariantObject* pElement, CRect& rectParent, CRect& rect);
	STDMETHOD(FitToParentEnd)(IVariantObject* pElement, CRect& rectParent, CRect& rect);

	STDMETHOD(BuildContainerInternal)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem, ElementType containerElementType);
	STDMETHOD(BuildHorizontalContainer)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(BuildVerticalContainer)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(BuildTextColumn)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(BuildTextMultiColumn)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(BuildImageColumn)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem);
	STDMETHOD(BuildMarqueeProgressColumn)(HDC hdc, RECT* pSourceRect, RECT* pDestRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IColumnsInfo* pColumnInfo, IColumnsInfoItem** ppColumnsInfoItem);

	STDMETHOD(GetElementType)(IVariantObject* pVariantObject, ElementType* pElementType);
	STDMETHOD(ApplyAlignHorizontal)(IColumnsInfo* pChildItems, CRect& rect, ElementType& elementType);
	STDMETHOD(ApplyAlignVertical)(IColumnsInfo* pChildItems, CRect& rect, ElementType& elementType);
	STDMETHOD(TranslateRects)(POINT* ptOrigin, IColumnsInfo* pColumnsInfo);
	STDMETHOD(VarToString)(CComVar& v, CComBSTR& bstr);
	STDMETHOD(CalculateRelativeWidth)(IVariantObject* pElement, CRect& rect, CRect& rectParent);

public:
	static HRESULT MapType(BSTR bstrType, ElementType* pElementType);

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);

	STDMETHOD(BuildLayout)(HDC hdc, RECT* pSourceRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo);
};

OBJECT_ENTRY_AUTO(__uuidof(LayoutBuilder), CLayoutBuilder)