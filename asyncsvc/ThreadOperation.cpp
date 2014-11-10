#include "stdafx.h"
#include "ThreadOperation.h"
#include <concrt.h>

CThreadOperation::CThreadOperation()
{
}

CThreadOperation::~CThreadOperation()
{
}

void CThreadOperation::Stop()
{
	if (m_pThread && m_pThread->joinable())
	{
		m_pThread->detach();
	}
	m_stop = true;
}

void CThreadOperation::Start()
{
	if (m_pThread && m_pThread->joinable())
	{
		m_pThread->join();
		m_pThread.reset();
	}
	m_pThread = make_shared<thread>(&CThreadOperation::Run, this);
}

void CThreadOperation::Run()
{
	shared_ptr<thread> pThread = m_pThread;
	OnRun();
	OnStop();
	if (pThread && pThread->joinable())
		pThread->detach();
}