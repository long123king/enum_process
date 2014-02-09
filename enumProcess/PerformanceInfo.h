#ifndef __ENUM_PROCESS_PERFORMANCE_INFO_H__
#define __ENUM_PROCESS_PERFORMANCE_INFO_H__

#include "tinyxml.h"
#include "report.h"

class PerformanceInfo
{
public:
	static PerformanceInfo* GetInstance()
	{
		static PerformanceInfo _instance;
		return &_instance;
	}

	void Initialize();
	void Enumerate();
	void Finalize();
	
private:
	PerformanceInfo();
	~PerformanceInfo();

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	bool m_bWork;
};

#endif//__ENUM_PROCESS_PERFORMANCE_INFO_H__