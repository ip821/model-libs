#pragma once

class CFindFileHandle
{
private:
	HANDLE m_H;

public:
	CFindFileHandle(HANDLE h)
	{
		m_H = h;
	}
	
	~CFindFileHandle()
	{
		if(m_H == NULL)
			return;

		if(m_H == INVALID_HANDLE_VALUE)
			return;

		::FindClose(m_H);
		m_H = NULL;
	}

	inline operator HANDLE() const
	{
		return m_H;
	}
};

