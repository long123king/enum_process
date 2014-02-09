#ifndef __ENUM_PROCESS_TH_THREAD_ENUMERATOR_H__
#define __ENUM_PROCESS_TH_THREAD_ENUMERATOR_H__

#include "IPidReceiver.h"
#include "tinyxml.h"
#include "report.h"

class thThreadEnumerator : public IPidReceiver
{
public:
	static thThreadEnumerator* GetInstance()
	{
		static thThreadEnumerator _instance;
		return &_instance;
	}	

	virtual void InitializeReceiver();
	virtual void ReceivePid( DWORD dwPid );
	virtual void FinalizeReceiver();

private:
	thThreadEnumerator();
	~thThreadEnumerator();

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	BOOL m_bWork;
};

#endif//__ENUM_PROCESS_TH_THREAD_ENUMERATOR_H__