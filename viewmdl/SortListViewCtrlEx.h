#pragma once

class CSortListViewCtrlEx : public CSortListViewCtrlImpl<CSortListViewCtrlEx>
{
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_SortListViewCtrl"), GetWndClassName())

	BEGIN_MSG_MAP(CSortListViewCtrlEx)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
		NOTIFY_CODE_HANDLER(HDN_ITEMCLICK, OnHeaderClick)
		CHAIN_MSG_MAP(CSortListViewCtrlImpl<CSortListViewCtrlEx>)
	END_MSG_MAP()

	struct NMMSG
	{
		NMHDR hdr;
		WPARAM wParam;
		LPARAM lParam;
	};

	LRESULT OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int nID = GetDlgCtrlID();
		NMMSG nm = { {m_hWnd, (UINT_PTR)GetDlgCtrlID(), uMsg}, wParam, lParam };

		::SendMessage(GetParent(), WM_NOTIFY, (WPARAM)nID, (LPARAM)&nm);
		return 0;
	}

	LRESULT OnHeaderClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		LPNMHEADER pNMLV = (LPNMHEADER)pnmh;
		m_bSortDescending = !m_bSortDescending;
		SetSortColumn(pNMLV->iItem);

		NotifyParentSortChanged(pNMLV->iItem, -1);
		return 0;
	}
};
