#ifndef __ENUM_PROCESS_MEMORY_INFO_H__
#define __ENUM_PROCESS_MEMORY_INFO_H__

#include "report.h"
#include "tinyxml.h"
#include "IPidReceiver.h"

class ProcessMemoryInfo : public IPidReceiver
{
public:
	static ProcessMemoryInfo* GetInstance()
	{
		static ProcessMemoryInfo _instance;
		return &_instance;
	}

	virtual void InitializeReceiver();
	virtual void ReceivePid(DWORD dwPid);
	virtual void FinalizeReceiver();

private:
	ProcessMemoryInfo();
	~ProcessMemoryInfo();

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	bool m_bWork;
};

#endif//__ENUM_PROCESS_MEMORY_INFO_H__