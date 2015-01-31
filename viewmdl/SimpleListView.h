// SimpleListView.h : Declaration of the CSimpleListView

#pragma once
#include "resource.h"       // main symbols

#include <vector>
#include "SortListViewCtrlEx.h"
#include "EditEx.h"
#include "viewmdl_contract_i.h"

using namespace ATL;
using namespace IP::ObjectModel;

// CSimpleListView

class ATL_NO_VTABLE CSimpleListView :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSimpleListView, &CLSID_SimpleListView>,
	public ISimpleListView,
	public CAxDialogImpl<CSimpleListView>,
	public CDialogResize<CSimpleListView>,
	public IInitializeWithControlImpl,
	public IControl2,
	public IInitializeWithVariantObject
{
public:
	enum {IDD = IDD_SIMPLELISTVIEW};

	CSimpleListView() :
		m_bSuspendEvents(FALSE)
	{
		m_listPopupMenu.CreatePopupMenu();
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_NO_REGISTRY()

	HRESULT FinalConstruct();
	void FinalRelease();

	BEGIN_COM_MAP(CSimpleListView)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ISimpleListView)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_IServiceProvider, m_pControl)
		COM_INTERFACE_ENTRY_AGGREGATE(IID_ICommandSupport, m_pCommandSupport)
	END_COM_MAP()

	BEGIN_DLGRESIZE_MAP(CSimpleListView)
		DLGRESIZE_CONTROL(IDC_EDIT1, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LIST1, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CSimpleListView)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		COMMAND_HANDLER(IDC_COMBO1, CBN_SELCHANGE, OnSelectionChanged)
		NOTIFY_HANDLER(IDC_LIST1, SLVN_SORTCHANGED, OnSortChanged)
		NOTIFY_HANDLER(IDC_LIST1, LVN_GETDISPINFO, OnGetDispInfo)
		NOTIFY_HANDLER(IDC_LIST1, WM_CONTEXTMENU, OnContextMenu)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CDialogResize<CSimpleListView>)
		CHAIN_MSG_MAP(CAxDialogImpl<CSimpleListView>)
	END_MSG_MAP()

private:
	struct _column_data
	{
		CString strColumnKey;
		CString strColumnName;
		int subItemIndex;
		VARTYPE vt;
		UINT uiWidth;
	};

	CSortListViewCtrlEx m_listView;
	CComboBox m_comboBox;
	CEditEx m_edit;
	std::vector<_column_data> m_columnsData;
	CString m_strCaption;
	CComPtr<IVariantTable> m_pVariantTable;
	BOOL m_bSuspendEvents;
	CMenu m_listPopupMenu;
	CComPtr<IPluginManager> m_pPluginManager;
	CComPtr<IPluginSupport> m_PluginSupport;
	CComPtr<ICommandSupport> m_pCommandSupport;
	CComPtr<ICommand> m_pCopyCommand;

	LRESULT OnContextMenu(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnSelectionChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSortChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnGetDispInfo(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEditKeyDown(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	STDMETHOD(AddColumns)(IVariantObject* pVariantObject);
	STDMETHOD(AddItems)(IVariantObject* pVariantObject);
	STDMETHOD(UpdateCaption)(IVariantObject* pVariantObject);
	STDMETHOD(AutoResizeColumns)();
	STDMETHOD(RefreshList)();
	STDMETHOD(RefreshFilter)();
public:
	struct _sort_data
	{
		CListViewCtrl* pListView;
		BOOL bAscending;
		int iSubItem;
	};

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(CreateEx2)(HWND hWndParent, RECT rect, HWND* hWnd);
	STDMETHOD(GetText)(BSTR* pbstr);
	STDMETHOD(OnActivate)();
	STDMETHOD(OnDeactivate)();
	STDMETHOD(OnClose)();

	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);

	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult);
	STDMETHOD(SetFilterVisible)(BOOL bVisible);

};

OBJECT_ENTRY_AUTO(__uuidof(SimpleListView), CSimpleListView)
