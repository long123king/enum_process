#ifndef __ENUM_PROCESS_TH_MODULE_ENUMERATOR_H__
#define __ENUM_PROCESS_TH_MODULE_ENUMERATOR_H__

#include "tinyxml.h"
#include "report.h"
#include "IPidReceiver.h"

class thModuleEnumerator : public IPidReceiver
{
public:
	static thModuleEnumerator* GetInstance()
	{
		static thModuleEnumerator _instance;
		return &_instance;
	}

	virtual void InitializeReceiver();
	virtual void ReceivePid( DWORD dwPid );
	virtual void FinalizeReceiver();	

private:
	thModuleEnumerator();
	~thModuleEnumerator();

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	BOOL m_bWork;

};

#endif//__ENUM_PROCESS_TH_MODULE_ENUMERATOR_H__