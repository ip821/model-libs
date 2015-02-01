#pragma once

namespace IP
{
	namespace ObjectModel
	{
		namespace Metadata
		{
			namespace Object
			{
				const static CComBSTR Name(L"VAR_NAME");
				const static CComBSTR Description(L"VAR_DESCRIPTION");
				const static CComBSTR Count(L"VAR_COUNT");
				const static CComBSTR Size(L"VAR_SIZE");
				const static CComBSTR Id(L"VAR_ID");
				const static CComBSTR Key(L"VAR_KEY");
				const static CComBSTR Value(L"VAR_VALUE");
				const static CComBSTR Type(L"VAR_OBJECT_TYPE");
			}

			namespace Table
			{
				namespace Column
				{
					const static CComBSTR Key(L"VAR_COLUMN_KEY");
					const static CComBSTR Name(L"VAR_COLUMN_NAME");
					const static CComBSTR Type(L"VAR_COLUMN_TYPE");
					const static CComBSTR Width(L"VAR_COLUMN_WIDTH");
				}
			}

			namespace TableObject
			{
				const static CComBSTR ColumnsObject(L"VAR_COLUMNS");
				const static CComBSTR ObjectsObject(L"VAR_OBJECTS");
			}


			namespace Plugins
			{
				namespace Object
				{
					const static CComBSTR Namespace(L"VAR_GUID_NAMESPACE");
					const static CComBSTR Type(L"VAR_GUID_TYPE");
					const static CComBSTR Id(L"VAR_GUID_ID");
					const static CComBSTR Clsid(L"VAR_GUID_CLSID");
					const static CComBSTR ClassFactoryObject(L"VAR_OBJECT_CLASSFACTORY");
				}
			}
		}
	}
}