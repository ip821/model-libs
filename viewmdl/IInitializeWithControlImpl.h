#pragma once

using namespace ATL;

#include "viewmdl_contract_i.h"

class ATL_NO_VTABLE IInitializeWithControlImpl : public IInitializeWithControl
{
protected:
	IInitializeWithControlImpl()
	{
		m_hControlWnd = 0;
	}

	CComPtr<IControl> m_pControl;
	HWND m_hControlWnd;
public:
	STDMETHOD(SetControl)(IControl* pControl)
	{
		m_pControl = pControl;
		if (m_pControl == NULL)
		{
			m_hControlWnd = 0;
		}
		else
		{
			RETURN_IF_FAILED(m_pControl->GetHWND(&m_hControlWnd));
		}
		return S_OK;
	}

	STDMETHOD(OnShutdown)()
	{
		m_hControlWnd = 0;
		m_pControl.Release();
		return S_OK;
	}
};
