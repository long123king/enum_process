#ifndef __ENUM_PROCESS_DRIVER_ENUMERATOR_H__
#define __ENUM_PROCESS_DRIVER_ENUMERATOR_H__

#include "report.h"
#include "tinyxml.h"

class DriverEnumerator
{
public:
	static DriverEnumerator* GetInstance()
	{
		static DriverEnumerator _instance;
		return &_instance;
	}

	void Initialize();
	void Enumerate();
	void Finalize();

private:
	DriverEnumerator();
	~DriverEnumerator();	

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	bool m_bWork;
};

#endif//__ENUM_PROCESS_DRIVER_ENUMERATOR_H__