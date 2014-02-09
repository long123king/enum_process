#ifndef __TH_ENUM_PROCESS_PROCESS_ENUMERATOR_H__
#define __TH_ENUM_PROCESS_PROCESS_ENUMERATOR_H__

#include <vector>
#include "IPidReceiver.h"
#include "tinyxml.h"
#include "report.h"

class thProcessEnumerator
{
public:
	static thProcessEnumerator* GetInstance()
	{
		static thProcessEnumerator _instance;
		return &_instance;
	}

	void AddReceiver(IPidReceiver* receiver);
	void Enumerate();

private:
	thProcessEnumerator();
	~thProcessEnumerator();

	void InitializeReceivers();
	void SendPid2Receivers(DWORD dwPid);
	void FinalizeReceivers();

private:
	std::vector<IPidReceiver*> m_vecReceivers;
	HANDLE m_hSnapshot;
	BOOL m_bWork;
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
};

#endif//__TH_ENUM_PROCESS_PROCESS_ENUMERATOR_H__