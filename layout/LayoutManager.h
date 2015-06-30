#pragma once

#include "layout_i.h"
#include "ElementType.h"

using namespace ATL;

class ATL_NO_VTABLE CLayoutManager :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLayoutManager, &CLSID_LayoutManager>,
	public ILayoutManager
{
public:
	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CLayoutManager)
		COM_INTERFACE_ENTRY(ILayoutManager)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT();

	void FinalRelease(){}
	HRESULT FinalConstruct();

private:
	CComPtr<ILayoutBuilder> m_pLayoutBuilder;
	CComPtr<ILayoutPainter> m_pLayoutPainter;

	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IImageManagerService> m_pImageManagerService;

	map<CComBSTR, CComPtr<IVariantObject>> m_layoutsMap;
	map<CComBSTR, CComPtr<IVariantObject>> m_stylesMap;

	STDMETHOD(ApplyStyle)(IVariantObject* pElement, BSTR bstrStyleName);
	STDMETHOD(ApplyStyles)(IVariantObject* pParentObject, IObjArray* pLayouts);
	STDMETHOD(CopyProps)(IVariantObject* pSourceObject, IVariantObject* pDestObject);

public:
	STDMETHOD(BuildLayout)(HDC hdc, RECT* pSourceRect, IVariantObject* pLayoutObject, IVariantObject* pValueObject, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo);
	STDMETHOD(EraseBackground)(HDC hdc, IColumnsInfo* pColumnInfo);
	STDMETHOD(PaintLayout)(HDC hdc, IImageManagerService* pImageManagerService, IColumnsInfo* pColumnInfo, BSTR bstrItemName);

	STDMETHOD(LoadThemeFromStream)(IStream* pStream);
	STDMETHOD(GetColorMap)(IThemeColorMap** ppThemeColorMap);
	STDMETHOD(GetFontMap)(IThemeFontMap** ppThemeFontMap);
	STDMETHOD(GetLayout)(BSTR bstrLayoutName, IVariantObject** ppVariantObject);
	STDMETHOD(GetImageManagerService)(IImageManagerService** ppImageManagerService);

};

OBJECT_ENTRY_AUTO(__uuidof(LayoutManager), CLayoutManager)