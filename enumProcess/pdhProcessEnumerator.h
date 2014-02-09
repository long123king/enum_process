#ifndef __ENUM_PROCESS_PDH_PROCESS_ENUMERATOR_H__
#define __ENUM_PROCESS_PDH_PROCESS_ENUMERATOR_H__

#include "report.h"
#include <Windows.h>

class pdhProcessEnumerator
{
public:
	static pdhProcessEnumerator* GetInstance()
	{
		static pdhProcessEnumerator _instance;
		return &_instance;
	}

	void Initialize();
	void Enumerate();
	void Finalize();

private:
	pdhProcessEnumerator();
	~pdhProcessEnumerator();

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	BOOL m_bWork;
};

#endif//__ENUM_PROCESS_PDH_PROCESS_ENUMERATOR_H__