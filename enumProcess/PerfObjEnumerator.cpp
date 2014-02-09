#include "PerfObjEnumerator.h"
#include "PdhSample.h"

void PerfObjEnumerator::Initialize()
{
	m_doc = Report::GetDocument("PerfObj.xml");
	m_root = new TiXmlElement("objects");
}

void PerfObjEnumerator::Enumerate()
{
	HKEY hPerf = NULL;
	PdhSample::ConnectComputerPerformanceRegistry(L"Thinkpad", hPerf);

	std::map<DWORD, LPWSTR> strMap;
	PdhSample::GetNameStrings(hPerf, strMap);

	std::vector<PERF_OBJECT_TYPE> vecObj;
	PdhSample::EnumPerfObjects(hPerf, vecObj);

	char szBuffer[1024] = {0};
	TiXmlElement* node = NULL;

	for (int i=0;i<vecObj.size();i++)
	{
		LPWSTR lpName = strMap[vecObj.at(i).ObjectNameTitleIndex];
		if(lpName == NULL)
			continue;
		WideCharToMultiByte(
			CP_ACP,
			NULL,
			lpName,
			wcslen(lpName),
			szBuffer,
			1024,
			NULL,
			NULL);

		node = new TiXmlElement("object");;
		node->SetAttribute("name", szBuffer);

		TiXmlElement* subNode = NULL;

		subNode = new TiXmlElement("numCounters");
		subNode->SetAttribute("value", vecObj.at(i).NumCounters);
		node->LinkEndChild(subNode);

		subNode = new TiXmlElement("numInstances");
		subNode->SetAttribute("value", vecObj.at(i).NumInstances);
		node->LinkEndChild(subNode);

		m_root->LinkEndChild(node);
	}

	PdhSample::LoadObjectData(hPerf, 6, strMap);
}

void PerfObjEnumerator::Finalize()
{
	if (m_bWork)
	{
		m_doc->LinkEndChild(m_root);
		m_doc->SaveFile();

		m_doc->Clear();
		delete m_doc;
		m_root = NULL;
		m_doc = NULL;
	}
}

PerfObjEnumerator::PerfObjEnumerator()
	:m_bWork(TRUE)
{

}

PerfObjEnumerator::~PerfObjEnumerator()
{

}
