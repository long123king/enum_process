#ifndef __ENUM_PROCESS_TH_HEAP_ENUMERATOR_H__
#define __ENUM_PROCESS_TH_HEAP_ENUMERATOR_H__

#include "tinyxml.h"
#include "report.h"
#include "IPidReceiver.h"

class thHeapEnumerator : public IPidReceiver
{
public:
	static thHeapEnumerator* GetInstance()
	{
		static thHeapEnumerator _instance;
		return &_instance;
	}

	virtual void InitializeReceiver();
	virtual void ReceivePid(DWORD dwPid);
	virtual void FinalizeReceiver();

private:
	thHeapEnumerator();
	~thHeapEnumerator();

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	BOOL m_bWork;

};

#endif//__ENUM_PROCESS_TH_HEAP_ENUMERATOR_H__