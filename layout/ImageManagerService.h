// ImageManagerService.h : Declaration of the CImageManagerService

#pragma once
#include "resource.h"       // main symbols
#include "layout_i.h"

using namespace ATL;
using namespace std;

// CImageManagerService

class ATL_NO_VTABLE CImageManagerService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CImageManagerService, &CLSID_ImageManagerService>,
	public IImageManagerService
{
public:
	CImageManagerService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CImageManagerService)
		COM_INTERFACE_ENTRY(IImageManagerService)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{

	}

private:
	std::map<CComBSTR, std::shared_ptr<Gdiplus::Bitmap>> m_bitmaps;
	boost::mutex m_mutex;
public:

	STDMETHOD(GetImageInfo)(BSTR bstrKey, TBITMAP* phBitmap);
	STDMETHOD(CreateImageBitmap)(BSTR bstrKey, HBITMAP* ptBitmap);
	STDMETHOD(AddImageFromFile)(BSTR bstrKey, BSTR bstrFileName);
	STDMETHOD(AddImageFromStream)(BSTR bstrKey, IStream* pStream);
	STDMETHOD(AddImageFromHBITMAP)(BSTR bstrKey, HBITMAP hBitmap);
	STDMETHOD(ContainsImageKey)(BSTR bstrKey, BOOL* pbContains);
	STDMETHOD(RemoveImage)(BSTR bstrKey);
	STDMETHOD(CopyImageTo)(BSTR bstrKey, IImageManagerService* pDest);
	STDMETHOD(CopyTo)(IImageManagerService* pDest);
	STDMETHOD(SaveImage)(BSTR bstrKey, BSTR bstrFilePath);
	STDMETHOD(Initialize)(IObjCollection* pImagesCollection);
};

OBJECT_ENTRY_AUTO(__uuidof(ImageManagerService), CImageManagerService)
