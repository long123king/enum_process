#ifndef __ENUM_PROCESS_PDH_OBJECT_ENUMERATOR_H__
#define __ENUM_PROCESS_PDH_OBJECT_ENUMERATOR_H__

#include "report.h"

class PerfObjEnumerator
{
public:
	static PerfObjEnumerator* GetInstance()
	{
		static PerfObjEnumerator _instance;
		return &_instance;
	}

	void Initialize();
	void Enumerate();
	void Finalize();

private:
	PerfObjEnumerator();
	~PerfObjEnumerator();

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	BOOL m_bWork;
};

#endif//__ENUM_PROCESS_PDH_OBJECT_ENUMERATOR_H__