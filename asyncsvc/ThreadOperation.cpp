#include "stdafx.h"
#include "ThreadOperation.h"

CThreadOperation::CThreadOperation()
{

}

CThreadOperation::~CThreadOperation()
{
}

void CThreadOperation::Stop()
{
	if (m_thread.joinable())
		m_thread.join();
	m_stop = true;
}

void CThreadOperation::Start()
{
	if (m_thread.joinable())
		m_thread.join();

	m_thread = std::thread(&CThreadOperation::Run, this);
}

void CThreadOperation::Run()
{
	OnRun();
	OnStop();
	if (m_thread.joinable())
		m_thread.detach();
}