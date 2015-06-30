// dllmain.h : Declaration of module class.

class ClayoutModule : public ATL::CAtlDllModuleT< ClayoutModule >
{
public :
	DECLARE_LIBID(LIBID_layoutLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_LAYOUT, "{660F7F3D-97B8-4EC1-AE70-CBB30D22C9C7}")
};

extern class ClayoutModule _AtlModule;
