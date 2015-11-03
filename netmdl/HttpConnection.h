#pragma once

#include "netmdl_i.h"

using namespace ATL;
using namespace std;

// CHttpConnection

class ATL_NO_VTABLE CHttpConnection :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CHttpConnection, &CLSID_HttpConnection>,
    public IHttpConnection
{
public:
    CHttpConnection()
    {
    }

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CHttpConnection)
        COM_INTERFACE_ENTRY(IHttpConnection)
    END_COM_MAP()

private:

protected:

public:

    STDMETHOD(RemoteCall)();
};

OBJECT_ENTRY_AUTO(__uuidof(HttpConnection), CHttpConnection)
