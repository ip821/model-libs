// AboutDialog.cpp : Implementation of CAboutDialog

#include "stdafx.h"
#include "AboutDialog.h"
#include "Functions.h"

// CAboutDialog

STDMETHODIMP CAboutDialog::DoModal(HWND hWnd, INT_PTR* pResult)
{
	CHECK_E_POINTER(pResult);
	*pResult = __super::DoModal(hWnd);
	return S_OK;
}

LRESULT CAboutDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CAxDialogImpl<CAboutDialog>::OnInitDialog(uMsg, wParam, lParam, bHandled);
	
	CWindow wndString = GetDlgItem(IDC_STATIC_APPNAME);

	HFONT hFont = wndString.GetFont();
	LOGFONT logFont = {0};
	::GetObject(hFont, sizeof(LOGFONT), (LPVOID)&logFont);
	logFont.lfHeight = -14;
	m_font.CreateFontIndirect(&logFont);
	wndString.SetFont(m_font);

	RETURN_IF_FAILED(HrGetPluginManager(&m_pPluginManager));
	m_pPluginManager->CoCreateInstance(CLSID_SimpleListView, IID_ISimpleListView, (LPVOID*)&m_pSimpleListView);
	CComQIPtr<IControl> pControl = m_pSimpleListView;
	RETURN_IF_FAILED(InsertToGroupboxPlaceholder(pControl, IDC_PLACEHOLDER2));
	RETURN_IF_FAILED(InitListView());
	RETURN_IF_FAILED(m_pSimpleListView->SetFilterVisible(FALSE));

	TCHAR lpszPath[MAX_PATH];
	if (!GetModuleFileName(NULL, lpszPath, MAX_PATH))
		return 0;

	DWORD dwVersionSize = GetFileVersionInfoSize(lpszPath, 0);
	CAutoVectorPtr<BYTE> pBuffer;
	pBuffer.Allocate(dwVersionSize);

	GetFileVersionInfo(lpszPath, 0, dwVersionSize, pBuffer);

	UINT uLen = 0;
	VS_FIXEDFILEINFO* pFileVersion = NULL;
	VerQueryValue(pBuffer, L"\\", (LPVOID*)&pFileVersion, &uLen);
	CString strVersion;
	strVersion.Format(
		L"%u.%u.%u.%u",
		(pFileVersion->dwFileVersionMS & 0xFFFF0000) >> 16,
		(pFileVersion->dwFileVersionMS & 0x0000FFFF),
		(pFileVersion->dwFileVersionLS & 0xFFFF0000) >> 16,
		(pFileVersion->dwFileVersionLS & 0x0000FFFF)
		);
#ifdef DEBUG
	strVersion += L" Debug";
#endif

	if(sizeof(INT_PTR) == 8)
	{
		strVersion += L" x64";
	}

	CWindow wndVersion = GetDlgItem(IDC_STATIC_VERSION);
	wndVersion.SetWindowText(strVersion);

	auto lpszName = PathFindFileName(lpszPath);
	PathRemoveExtension(lpszName);
	wndString.SetWindowText(lpszName);

	CenterWindow(GetParent());
	bHandled = TRUE;
	return 1;  // Let the system set the focus
}

STDMETHODIMP CAboutDialog::InitListView()
{
	CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = m_pSimpleListView;
	ATLASSERT(pInitializeWithVariantObject);
	CComPtr<IObjArray> pObjectArray;
	RETURN_IF_FAILED(m_pPluginManager->GetPluginInfoCollection(RAWOBJECT, RAWOBJECT, &pObjectArray));
	CComPtr<IVariantObject> pVariantObject;
	RETURN_IF_FAILED(m_pPluginManager->CoCreateInstance(CLSID_VariantObject, IID_IVariantObject, (LPVOID*)&pVariantObject));

	CComPtr<IVariantTable> pVariantTableColumns;
	RETURN_IF_FAILED(m_pPluginManager->CoCreateInstance(CLSID_VariantTable, IID_IVariantTable, (LPVOID*)&pVariantTableColumns));
	RETURN_IF_FAILED(HrAddColumn(m_pPluginManager, pVariantObject, IP::ObjectModel::Metadata::Object::Name, L"Name", VT_BSTR, 500));

	CComPtr<IVariantTable> pVariantTableObjects;
	RETURN_IF_FAILED(m_pPluginManager->CoCreateInstance(CLSID_VariantTable, IID_IVariantTable, (LPVOID*)&pVariantTableObjects));
	RETURN_IF_FAILED(pVariantTableObjects->SetObjects(pObjectArray));
	RETURN_IF_FAILED(pVariantTableObjects->SetSort(IP::ObjectModel::Metadata::Object::Name, FALSE));

	RETURN_IF_FAILED(pVariantObject->SetVariantValue(IP::ObjectModel::Metadata::TableObject::ColumnsObject, &CComVariant(pVariantTableObjects)));

	pInitializeWithVariantObject->SetVariantObject(pVariantObject);
	return S_OK;
}

LRESULT CAboutDialog::OnClickedButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
