#include "ProcessMemoryInfo.h"

#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

ProcessMemoryInfo::ProcessMemoryInfo()
	:m_bWork(true)
{

}

ProcessMemoryInfo::~ProcessMemoryInfo()
{


}

void ProcessMemoryInfo::InitializeReceiver()
{
	m_doc = Report::GetDocument("processMemoryInfo.xml");
	if (m_doc == NULL)
	{
		m_bWork = false;
	}

	if (m_bWork)
	{
		m_root = new TiXmlElement("processes");
	}
}

void ProcessMemoryInfo::ReceivePid( DWORD dwPid )
{
	//printf("********************\nPID: %d\n", dwPid);
	char szPid[16] = {0};
	sprintf(szPid, "%d", dwPid);


	TiXmlElement* process = new TiXmlElement("process");
	process->SetAttribute("PID", dwPid);

	HANDLE hProcess = OpenProcess(
		PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE,
		dwPid);

	if (INVALID_HANDLE_VALUE == hProcess)
	{
		delete process;
		return;
	}

	HMODULE hNtdll = LoadLibrary(L"Ntdll.dll");
	if (INVALID_HANDLE_VALUE == hNtdll)
	{
		delete process;
		return;
	}

	PROCESS_MEMORY_COUNTERS pmc;
	DWORD dwLen = sizeof(pmc);
	BOOL bRet = GetProcessMemoryInfo(
		hProcess,
		&pmc,
		dwLen);

	TiXmlElement* node = new TiXmlElement("PageFaultCount");
	node->SetAttribute("value", pmc.PageFaultCount);
	process->LinkEndChild(node);

	node = new TiXmlElement("PeakWorkingSetSize");
	node->SetAttribute("value", pmc.PeakWorkingSetSize);
	process->LinkEndChild(node);

	node = new TiXmlElement("WorkingSetSize");
	node->SetAttribute("value", pmc.WorkingSetSize);
	process->LinkEndChild(node);

	node = new TiXmlElement("QuotaPeakPagedPoolUsage");
	node->SetAttribute("value", pmc.QuotaPeakPagedPoolUsage);
	process->LinkEndChild(node);

	node = new TiXmlElement("QuotaPagedPoolUsage");
	node->SetAttribute("value", pmc.QuotaPagedPoolUsage);
	process->LinkEndChild(node);

	node = new TiXmlElement("QuotaPeakNonPagedPoolUsage");
	node->SetAttribute("value", pmc.QuotaPeakNonPagedPoolUsage);
	process->LinkEndChild(node);

	node = new TiXmlElement("QuotaNonPagedPoolUsage");
	node->SetAttribute("value", pmc.QuotaNonPagedPoolUsage);
	process->LinkEndChild(node);

	node = new TiXmlElement("PagefileUsage");
	node->SetAttribute("value", pmc.PagefileUsage);
	process->LinkEndChild(node);

	node = new TiXmlElement("PeakPagefileUsage");
	node->SetAttribute("value", pmc.PeakPagefileUsage);
	process->LinkEndChild(node);

	m_root->LinkEndChild(process);
}

void ProcessMemoryInfo::FinalizeReceiver()
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
