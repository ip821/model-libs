#pragma once

#include <thread>
#include <atomic>

class CThreadOperation
{
protected:
	virtual void OnRun() = 0;
	virtual void OnStop() = 0;
	std::atomic<bool> m_stop;
	std::thread m_thread;

	void Run();
public:
	CThreadOperation();
	virtual ~CThreadOperation();

	CThreadOperation(CThreadOperation const&) = delete;
	CThreadOperation& operator =(CThreadOperation const&) = delete;

	void Stop();
	void Start();
};
