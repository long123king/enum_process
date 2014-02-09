#ifndef __ENUM_PROCESS_PROCESS_ENUMERATOR_H__
#define __ENUM_PROCESS_PROCESS_ENUMERATOR_H__

#include <vector>
#include "IPidReceiver.h"

class ProcessEnumerator
{
public:
	static ProcessEnumerator* GetInstance()
	{
		static ProcessEnumerator _instance;
		return &_instance;
	}

	void AddReceiver(IPidReceiver* receiver);
	void Enumerate();

private:
	ProcessEnumerator();
	~ProcessEnumerator();

	void InitializeReceivers();
	void SendPid2Receivers(DWORD dwPid);
	void FinalizeReceivers();

private:
	std::vector<IPidReceiver*> m_vecReceivers;
};

#endif//__ENUM_PROCESS_PROCESS_ENUMERATOR_H__