// dllmain.h : Declaration of module class.

class CViewMdlModule : public ATL::CAtlDllModuleT< CViewMdlModule >
{
public :
	DECLARE_LIBID(LIBID_ViewMdlLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_VIEWMDL, "{67D6F7F8-CBE6-4E0D-A786-5CC81914F59B}")
};

extern class CViewMdlModule _AtlModule;
