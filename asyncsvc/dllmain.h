// dllmain.h : Declaration of module class.

class CNotificationServicesModule : public ATL::CAtlDllModuleT< CNotificationServicesModule >
{
public :
	DECLARE_LIBID(LIBID_NotificationServicesLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_NOTIFICATIONSERVICES, "{17CB37A7-71AD-476B-AC54-D1D1A70BFD80}")
};

extern class CNotificationServicesModule _AtlModule;
