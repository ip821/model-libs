// SimpleListView.cpp : Implementation of CSimpleListView

#include "stdafx.h"
#include "SimpleListView.h"
#include "CopyCommand.h"

// CSimpleListView

HRESULT CSimpleListView::FinalConstruct()
{
	RETURN_IF_FAILED(HrGetPluginManager(&m_pPluginManager));
	RETURN_IF_FAILED(m_pPluginManager->CoCreateInstance(CLSID_PluginSupport, IID_IPluginSupport, (LPVOID*)&m_PluginSupport));
	RETURN_IF_FAILED(m_pPluginManager->CoCreateInstance(CLSID_CommandSupport, IID_ICommandSupport, (LPVOID*)&m_pCommandSupport));
	return S_OK;
}

void CSimpleListView::FinalRelease()
{
}

STDMETHODIMP CSimpleListView::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CSimpleListView::CreateEx2(HWND hWndParent, RECT rect, HWND* hWnd)
{
	CHECK_E_POINTER(hWnd);
	m_hWnd = __super::Create(hWndParent, rect);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CSimpleListView::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	m_hWnd = __super::Create(hWndParent);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CSimpleListView::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(pbResult);
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_EDIT1) && pMsg->wParam == VK_RETURN)
	{
		RefreshFilter();
		*pbResult = TRUE;
	}
	return S_OK;
}

STDMETHODIMP CSimpleListView::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult)
{
	CHECK_E_POINTER(bResult);
	*bResult = ProcessWindowMessage(hWnd, uMsg, wParam, lParam, *plResult);
	return S_OK;
}

LRESULT CSimpleListView::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bResult = FALSE;
	LRESULT lResult = 0;
	RETURN_IF_FAILED(m_pCommandSupport->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult));
	bHandled = bResult;
	return lResult;
}

LRESULT CSimpleListView::OnContextMenu(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	if (!GetMenuItemCount(m_listPopupMenu))
		return 0;

	LVITEM lvItem = { 0 };
	m_listView.GetSelectedItem(&lvItem);
	if (lvItem.iItem == -1)
		return 0;

	CSortListViewCtrlEx::NMMSG* pNM = (CSortListViewCtrlEx::NMMSG*)pnmh;
	auto xPos = GET_X_LPARAM(pNM->lParam);
	auto yPos = GET_Y_LPARAM(pNM->lParam);
	POINT pt = { xPos, yPos };
	CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = m_pCommandSupport;
	if (pInitializeWithVariantObject)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(m_pVariantTable->GetVariantObject(lvItem.iItem, &pVariantObject));
		RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(pVariantObject));
	}

	CComQIPtr<IInitializeWithColumnName> pInitializeWithColumnName = m_pCommandSupport;
	if (pInitializeWithColumnName)
	{
		POINT pt1 = pt;
		ScreenToClient(&pt1);
		LVHITTESTINFO hittest = { 0 };
		hittest.pt = pt1;
		m_listView.SubItemHitTest(&hittest);
		if (hittest.iSubItem == -1)
			return 0;
		auto column_data = m_columnsData[hittest.iSubItem];
		RETURN_IF_FAILED(pInitializeWithColumnName->SetColumnName(column_data.strColumnKey));
	}

	CComQIPtr<IIdleHandler> pIdleHandler = m_pCommandSupport;
	ATLASSERT(pIdleHandler);
	if (pIdleHandler)
	{
		BOOL bResult = FALSE;
		RETURN_IF_FAILED(pIdleHandler->OnIdle(&bResult));
	}
	m_listPopupMenu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);

	return 0;
}

LRESULT CSimpleListView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	DlgResize_Init(false);
	m_edit.SubclassWindow(GetDlgItem(IDC_EDIT1));
	m_comboBox.Attach(GetDlgItem(IDC_COMBO1));
	m_listView.SubclassWindow(GetDlgItem(IDC_LIST1));
	m_listView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	CComQIPtr<IInitializeWithControl> pInit1 = m_pCommandSupport;
	if (pInit1)
	{
		RETURN_IF_FAILED(pInit1->SetControl(this));
	}

	CComQIPtr<IInitializeWithControl> pInit2 = m_PluginSupport;
	if (pInit2)
	{
		RETURN_IF_FAILED(pInit2->SetControl(this));
	}

	m_pCommandSupport->SetMenu(m_listPopupMenu);

	bHandled = FALSE;
	return 0;
}

LRESULT CSimpleListView::OnGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMLVDISPINFO* pDispInfo = (NMLVDISPINFO*)pnmh;
	auto column_data = m_columnsData[pDispInfo->item.iSubItem];

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CComVariant v;
		RETURN_IF_FAILED(m_pVariantTable->GetVariantValue(pDispInfo->item.iItem, CComBSTR(column_data.strColumnKey), &v));

		switch (v.vt)
		{
		case VT_BSTR:
			_tcsncpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, v.bstrVal, pDispInfo->item.cchTextMax - 1);
			break;

		case VT_I8:
		{
					  std::wstring strValue = boost::lexical_cast<std::wstring>(v.llVal);
					  _tcsncpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, strValue.c_str(), pDispInfo->item.cchTextMax - 1);
		}
			break;

		case VT_I4:
		{
					  std::wstring strValue = boost::lexical_cast<std::wstring>(v.intVal);
					  _tcsncpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, strValue.c_str(), pDispInfo->item.cchTextMax - 1);
		}
			break;

		case VT_UI8:
		{
					  std::wstring strValue = boost::lexical_cast<std::wstring>(v.ullVal);
					  _tcsncpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, strValue.c_str(), pDispInfo->item.cchTextMax - 1);
		}
			break;

		default:
			break;
		}
	}

	return 0;
}

LRESULT CSimpleListView::OnSortChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
{
	CWaitCursor w;

	int colIndex = m_listView.GetSortColumn();
	BOOL bDescending = m_listView.IsSortDescending();
	auto column_data = m_columnsData[colIndex];
	m_pVariantTable->SetSort(CComBSTR(column_data.strColumnKey), bDescending);
	RefreshList();
	return 0;
}

STDMETHODIMP CSimpleListView::GetText(BSTR* pbstr)
{
	CHECK_E_POINTER(pbstr);
	*pbstr = CComBSTR(m_strCaption).Copy();
	return S_OK;
}

STDMETHODIMP CSimpleListView::OnActivate()
{
	return S_OK;
}

STDMETHODIMP CSimpleListView::OnDeactivate()
{
	return S_OK;
}

STDMETHODIMP CSimpleListView::OnClose()
{
	CComQIPtr<IInitializeWithControl> pInit1 = m_pCommandSupport;
	if (pInit1)
	{
		RETURN_IF_FAILED(pInit1->SetControl(NULL));
	}

	CComQIPtr<IInitializeWithControl> pInit2 = m_PluginSupport;
	if (pInit2)
	{
		RETURN_IF_FAILED(pInit2->SetControl(NULL));
	}

	RETURN_IF_FAILED(m_pCommandSupport->UninstallAll());
	RETURN_IF_FAILED(m_PluginSupport->UninstallAll());

	return S_OK;
}

STDMETHODIMP CSimpleListView::AddColumns(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComVariant vColumns;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Metadata::TableObject::ColumnsObject, &vColumns));
	if (vColumns.vt != (VT_UNKNOWN))
		return HRESULT_FROM_WIN32(ERROR_INVALID_VARIANT);

	CComPtr<IObjArray> pObjectArray;
	vColumns.punkVal->QueryInterface(IID_IObjArray, (LPVOID*)&pObjectArray);
	if (!pObjectArray)
		return E_NOINTERFACE;

	m_comboBox.AddString(L"<all columns>");
	m_bSuspendEvents = TRUE;
	m_comboBox.SetCurSel(0);
	m_bSuspendEvents = FALSE;

	UINT uCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uCount));
	for (UINT i = 0; i < uCount; i++)
	{
		CComPtr<IVariantObject> pVariantObjectValue;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, IID_IVariantObject, (LPVOID*)&pVariantObjectValue));
		CComVariant vColumnKey;
		pVariantObjectValue->GetVariantValue(Metadata::Table::Column::Key, &vColumnKey);
		CComVariant vColumnName;
		pVariantObjectValue->GetVariantValue(Metadata::Table::Column::Name, &vColumnName);
		CComVariant vColumnType;
		pVariantObjectValue->GetVariantValue(Metadata::Table::Column::Type, &vColumnType);
		CComVariant vColumnWidth;
		pVariantObjectValue->GetVariantValue(Metadata::Table::Column::Width, &vColumnWidth);

		m_listView.AddColumn(vColumnName.bstrVal, i);
		_column_data cd = { vColumnKey.bstrVal, vColumnName.bstrVal, i, vColumnType.uiVal, vColumnWidth.intVal };
		m_columnsData.push_back(cd);
		m_comboBox.AddString(cd.strColumnName);
	}
	return S_OK;
}

STDMETHODIMP CSimpleListView::RefreshList()
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pVariantTable->GetCount(&uiCount));
	m_listView.SetItemCount(uiCount);
	return S_OK;
}

STDMETHODIMP CSimpleListView::RefreshFilter()
{
	CWaitCursor w;
	int index = m_comboBox.GetCurSel() - 1;

	CString strEditText;
	m_edit.GetWindowText(strEditText);

	if (strEditText.IsEmpty())
	{
		m_pVariantTable->ResetFilter();
		RefreshList();
		return S_OK;
	}

	m_pVariantTable->SetFilter(
		index == -1 ? NULL : CComBSTR(m_columnsData[index].strColumnKey),
		&CComVariant(strEditText)
		);
	RefreshList();
	return S_OK;
}

LRESULT CSimpleListView::OnSelectionChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (m_bSuspendEvents)
		return 0;

	RefreshFilter();
	return 0;
}

STDMETHODIMP CSimpleListView::SetFilterVisible(BOOL bVisible)
{
	if (!m_edit.ShowWindow(bVisible ? SW_SHOW : SW_HIDE))
		return HRESULT_FROM_WIN32(GetLastError());
	if (!m_comboBox.ShowWindow(bVisible ? SW_SHOW : SW_HIDE))
		return HRESULT_FROM_WIN32(GetLastError());

	if (!m_edit.IsWindowVisible() && !m_comboBox.IsWindowVisible())
	{
		RECT rectCombo = { 0 };
		if (!m_comboBox.GetWindowRect(&rectCombo))
			return HRESULT_FROM_WIN32(GetLastError());

		RECT rect = { 0 };
		if (!m_listView.GetWindowRect(&rect))
			return HRESULT_FROM_WIN32(GetLastError());

		if (!ScreenToClient(&rect))
			return HRESULT_FROM_WIN32(GetLastError());

		rect.top += rectCombo.top - rectCombo.bottom;

		m_listView.MoveWindow(&rect);
	}

	return S_OK;
}

STDMETHODIMP CSimpleListView::AddItems(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComVariant vObjects;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Metadata::TableObject::ColumnsObject, &vObjects));
	if (vObjects.vt != (VT_UNKNOWN))
		return HRESULT_FROM_WIN32(ERROR_INVALID_VARIANT);

	RETURN_IF_FAILED(vObjects.punkVal->QueryInterface(IID_IVariantTable, (LPVOID*)&m_pVariantTable));
	RefreshList();
	return S_OK;
}

STDMETHODIMP CSimpleListView::UpdateCaption(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CComVariant vCaption;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Metadata::Object::Name, &vCaption));
	m_strCaption = vCaption.bstrVal;

	CComQIPtr<IContainerControl> pContainerControl = m_pControl;
	if (pContainerControl)
	{
		RETURN_IF_FAILED(pContainerControl->UpdateControl(this));
	}

	return S_OK;
}

STDMETHODIMP CSimpleListView::AutoResizeColumns()
{
	for (UINT i = 0; i < m_columnsData.size(); i++)
	{
		m_listView.SetColumnWidth(i, m_columnsData[i].uiWidth);
	}
	return S_OK;
}

STDMETHODIMP CSimpleListView::SetVariantObject(IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	RETURN_IF_FAILED(UpdateCaption(pVariantObject));
	RETURN_IF_FAILED(AddColumns(pVariantObject));
	RETURN_IF_FAILED(AddItems(pVariantObject));
	RETURN_IF_FAILED(AutoResizeColumns());

	CComVariant vId;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Metadata::Object::Id, &vId));
	if (vId.vt != VT_EMPTY)
	{
		GUID gId = { 0 };
		RETURN_IF_FAILED(CLSIDFromString(vId.bstrVal, &gId));
		HRESULT hr = m_PluginSupport->InitializePlugins(PVIEWTYPE_INPLACE_CONTROL, gId);
		if (FAILED(hr) && hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
			return hr;
		RETURN_IF_FAILED(m_PluginSupport->OnInitialized());
		RETURN_IF_FAILED(m_pCommandSupport->InstallCommands(m_PluginSupport));
	}

	RETURN_IF_FAILED(m_pPluginManager->CoCreateInstance(CLSID_CopyCommand, IID_ICommand, (LPVOID*)&m_pCopyCommand));
	RETURN_IF_FAILED(m_pCommandSupport->InstallCommand(m_pCopyCommand));

	return S_OK;
}
