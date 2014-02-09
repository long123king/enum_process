#ifndef __ENUM_PROCESS_PID_RECEIVER_H__
#define __ENUM_PROCESS_PID_RECEIVER_H__

#include <Windows.h>

class IPidReceiver
{
public:
	virtual void InitializeReceiver() = 0;
	virtual void ReceivePid(DWORD dwPid) = 0;
	virtual void FinalizeReceiver() = 0;
};

#endif//__ENUM_PROCESS_PID_RECEIVER_H__