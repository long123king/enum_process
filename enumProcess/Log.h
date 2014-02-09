#ifndef __ENUM_PROCESS_LOG_H__
#define __ENUM_PROCESS_LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

class Log
{
public:
	static Log* GetInstance()
	{
		static Log _instance;
		return &_instance;
	}

	void v(const char* cmd, ...);

	void SwitchOn(BOOL bOn)
	{
		m_bWork = bOn;
	}

private:
	Log();
	~Log();

private:
	BOOL m_bWork;
};

#define LOG Log::GetInstance()->v
#define LOG_ON Log::GetInstance()->SwitchOn(TRUE);
#define LOG_OFF Log::GetInstance()->SwitchOn(FALSE);

#endif//__ENUM_PROCESS_LOG_H__