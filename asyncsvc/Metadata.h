#pragma once

#include <comdef.h>

namespace IP
{
	namespace AsyncServices
	{
		namespace Metadata
		{
			namespace Timer
			{
				const static CComBSTR Interval(L"TimerInterval");
				const static CComBSTR RestartFlag(L"VAR_RESTART_TIMER");
			}

			namespace Thread
			{
				const static CComBSTR Id(L"VAR_THREAD_ID");
				const static CComBSTR HResult(L"hr");
				const static CComBSTR HResultDescription(L"VAR_HRESULT_DESCRIPTION");
			}
		}
	}
}