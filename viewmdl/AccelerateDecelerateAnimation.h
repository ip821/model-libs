#pragma once
#include "ViewMdl_i.h"

using namespace ATL;

// CAccelerateDecelerateAnimation

class ATL_NO_VTABLE CAccelerateDecelerateAnimation :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAccelerateDecelerateAnimation, &CLSID_AccelerateDecelerateAnimation>,
	public IAnimation,
	public IConnectionPointContainerImpl<CAccelerateDecelerateAnimation>,
	public IConnectionPointImpl<CAccelerateDecelerateAnimation, &__uuidof(IAnimationEventSink)>
{
public:
	CAccelerateDecelerateAnimation()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CAccelerateDecelerateAnimation)
		COM_INTERFACE_ENTRY(IAnimation)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CAccelerateDecelerateAnimation)
		CONNECTION_POINT_ENTRY(__uuidof(IAnimationEventSink))
	END_CONNECTION_POINT_MAP()

private:

	HRESULT Fire_OnAnimation();

public:

	STDMETHOD(SetParams)(DOUBLE dwFrom, DOUBLE dwTo, DOUBLE dwDuration);
	STDMETHOD(Run)();
	STDMETHOD(GetCurrentIntValue)(INT32* piVal);
	STDMETHOD(GetCurrentValue)(DOUBLE* pdblVal);
};

OBJECT_ENTRY_AUTO(__uuidof(AccelerateDecelerateAnimation), CAccelerateDecelerateAnimation)
