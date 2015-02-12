#pragma once

#include <atlwin.h>
#include <map>

template<class T>
class CChildDialogControlSupport
{
private:
	std::map<IControl*, HWND > m_childControlsMap;
public:
	STDMETHOD(InsertToGroupboxPlaceholder)(IControl* pControl, UINT uiPlaceholderId, BOOL bResize = TRUE)
	{
		T* pThis = static_cast<T*>(this);
		ATLENSURE(pThis);

		CWindow wPlaceholder = pThis->GetDlgItem(uiPlaceholderId);
		ATLENSURE(wPlaceholder.m_hWnd);

		RECT r = {0};
		ATLENSURE(wPlaceholder.GetWindowRect(&r));
		ATLENSURE(pThis->ScreenToClient(&r));
		HWND hWnd = 0;
		RETURN_IF_FAILED(pControl->CreateEx(pThis->m_hWnd, &hWnd));
		ATLENSURE(hWnd);

		m_childControlsMap[pControl] = hWnd;

		CWindow childWindow = hWnd;
		if(!bResize)
		{
			RECT rectChildWindow = {0};
			childWindow.GetWindowRect(&rectChildWindow);
			r.bottom = r.top + (rectChildWindow.bottom - rectChildWindow.top);
			r.right = r.left + (rectChildWindow.right - rectChildWindow.left);
		}

		ATLENSURE(childWindow.MoveWindow(&r));

		return S_OK;
	}
};