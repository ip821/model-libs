// dllmain.h : Declaration of module class.

class CObjMdlModule : public ATL::CAtlDllModuleT< CObjMdlModule >
{
public :
	DECLARE_LIBID(LIBID_ObjMdlLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_OBJMDL, "{A8DCD899-1233-4F4A-9A4C-80712C3D5E68}")
};

extern class CObjMdlModule _AtlModule;
