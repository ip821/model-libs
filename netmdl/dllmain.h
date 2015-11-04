// dllmain.h : Declaration of module class.

class CnetmdlModule : public ATL::CAtlDllModuleT< CnetmdlModule >
{
public :
	DECLARE_LIBID(LIBID_netmdlLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_NETMDL, "{41CF6EF0-542C-49DC-8FC1-F55F373A5AB3}")
};

extern class CnetmdlModule _AtlModule;
