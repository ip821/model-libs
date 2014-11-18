#include "stdafx.h"
#include "ThreadOperation.h"
#include <concrt.h>

CThreadOperation::CThreadOperation()
{
}

CThreadOperation::~CThreadOperation()
{
}

void CThreadOperation::JoinAndStop(bool bJoin)
{
	auto handle = m_handle.load();
	if (handle)
	{
		WaitForSingleObject(handle, bJoin ? 2000 : INFINITE);
		CloseHandle(handle);
	}
}

void CThreadOperation::Stop()
{
	m_stop = true;
	JoinAndStop(false);
}

unsigned __stdcall CThreadOperationThreadProc(void* pThis)
{
	CThreadOperation* pThreadOperation = static_cast<CThreadOperation*>(pThis);
	pThreadOperation->Run();
	return 0;
}

void CThreadOperation::Start()
{
	JoinAndStop();
	m_handle = (HANDLE)_beginthreadex(nullptr, 0, &CThreadOperationThreadProc, this, 0, nullptr);
}

void CThreadOperation::Run()
{
	OnRun();
	OnStop();
	auto handle = m_handle.load();
	m_handle = 0;
	CloseHandle(handle);
}