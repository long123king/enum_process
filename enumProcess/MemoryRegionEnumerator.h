#ifndef __ENUM_PROCESS_MEMORY_REGION_ENUMERATOR_H__
#define __ENUM_PROCESS_MEMORY_REGION_ENUMERATOR_H__

#include "IPidReceiver.h"
#include "tinyxml.h"
#include "report.h"
#include <winternl.h>   // for Windows internal declarations.

class MemoryRegionEnumerator : public IPidReceiver
{
public:
	static MemoryRegionEnumerator* GetInstance()
	{
		static MemoryRegionEnumerator _instance;
		return &_instance;
	}

	virtual void InitializeReceiver();
	virtual void ReceivePid(DWORD dwPid);
	virtual void FinalizeReceiver();

private:
	MemoryRegionEnumerator();
	~MemoryRegionEnumerator();

	void printParameters(PRTL_USER_PROCESS_PARAMETERS parameters,
		HANDLE hProcess,
		TiXmlElement* process);
	
	void printMemorySection(HANDLE hProcess, TiXmlElement* process);

	std::string ProtectString(DWORD value);

	std::string StateString(DWORD value);

	std::string TypeString(DWORD value);

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	bool m_bWork;
};

#endif//__ENUM_PROCESS_MEMORY_REGION_ENUMERATOR_H__