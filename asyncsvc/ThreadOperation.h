#pragma once

#include <thread>
#include <atomic>
#include <memory>

using namespace std;

class CThreadOperation
{
protected:
	virtual void OnRun() = 0;
	virtual void OnStop() = 0;
	atomic<bool> m_stop;
	shared_ptr<thread> m_pThread;

	void Run();

public:

	CThreadOperation();
	virtual ~CThreadOperation();

	CThreadOperation(CThreadOperation const&) = delete;
	CThreadOperation& operator =(CThreadOperation const&) = delete;

	void Stop();
	void Start();
};
