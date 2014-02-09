#include "DriverEnumerator.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

DriverEnumerator::DriverEnumerator()
	:m_bWork(true)
{

}

DriverEnumerator::~DriverEnumerator()
{

}

void DriverEnumerator::Enumerate()
{
	LPVOID szBuffer[1024] = {0};
	DWORD dwLen = 1024 * sizeof(LPVOID);
	bool bRet = EnumDeviceDrivers(szBuffer,
		dwLen,
		&dwLen);
	if (!bRet || dwLen > 1024 * sizeof(LPVOID))
	{
		return;
	}

	DWORD dwNums = dwLen / sizeof(LPVOID);
	for (int i=0;i<dwNums;i++)
	{
		char szName[1024] = {0};
		TiXmlElement* process = new TiXmlElement("driver");
		DWORD dwRet = GetDeviceDriverBaseNameA(
			szBuffer[i],
			szName,
			1024);

		if (dwRet > 1024 || dwRet == 0)
		{
			delete process;
			continue;
		}
		process->SetAttribute("Name", szName);
		dwRet = GetDeviceDriverFileNameA(
			szBuffer[i],
			szName,
			1024);

		if (dwRet > 1024 || dwRet == 0)
		{
			delete process;
			continue;
		}

		process->SetAttribute("Path", szName);
		m_root->LinkEndChild(process);

	}


}

void DriverEnumerator::Initialize()
{
	m_doc = Report::GetDocument("driver.xml");
	if (m_doc == NULL)
	{
		m_bWork = false;
	}

	if (m_bWork)
	{
		m_root = new TiXmlElement("Drivers");
	}	
}

void DriverEnumerator::Finalize()
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
