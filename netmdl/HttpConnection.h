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

    string m_callbackData;
    string m_strUrl;

    static int CurlCallback(char* data, size_t size, size_t nmemb, CHttpConnection* pObj);
    int SaveLastWebResponse(char*& data, size_t size);

    int m_cp = CP_THREAD_ACP;

public:

    STDMETHOD(RemoteCall)(BSTR bstrQuery, BSTR bstrUser, BSTR bstrPass, BSTR* pbstrResult);
    STDMETHOD(SetEncoding)(INT cp);
};

OBJECT_ENTRY_AUTO(__uuidof(HttpConnection), CHttpConnection)
