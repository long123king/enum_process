#include "MemoryRegionEnumerator.h"

#include <Psapi.h>
#pragma comment(lib, "psapi.lib")


std::string MemoryRegionEnumerator::ProtectString(DWORD value)
{
	const char* szProtection[] = {
		"PAGE_NOACCESS",
		"PAGE_READONLY",
		"PAGE_READWRITE",
		"PAGE_WRITECOPY",
		"PAGE_EXECUTE_NOACCESS",
		"PAGE_EXECUTE_READONLY",
		"PAGE_EXECUTE_READWRITE",
		"PAGE_EXECUTE_WRITECOPY",
		"PAGE_GUARD",
		"PAGE_NOCACHE",
		"PAGE_WRITECOMBINE"};

		std::string result;
		for (int i=0;i< sizeof(szProtection) / sizeof(char*);i++)
		{
			if (value >> i & 1)
			{
				if (result.length()!= 0)
					result += " | ";
				result += szProtection[i];
			}
		}
		return result;
}

std::string MemoryRegionEnumerator::StateString(DWORD value)
{
	std::string result;
	if (value == 0x1000)
	{
		result = "MEM_COMMIT";
	} 
	else if(value == 0x10000)
	{
		result = "MEM_FREE";
	}
	else if(value == 0x2000)
	{
		result = "MEM_RESERVE";
	}
	else
	{
		result = "";
	}
	return result;
}

std::string MemoryRegionEnumerator::TypeString(DWORD value)
{
	std::string result;
	if (value == 0x1000000)
	{
		result = "MEM_IMAGE";
	} 
	else if(value == 0x40000)
	{
		result = "MEM_MAPPED";
	}
	else if(value == 0x20000)
	{
		result = "MEM_PRIVATE";
	}
	else
	{
		result = "";
	}
	return result;
}

void MemoryRegionEnumerator::printMemorySection(HANDLE hProcess, TiXmlElement* process)
{
	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi, 0, sizeof(mbi));
	DWORD dwLen = sizeof(mbi);
	char* baseAddr = NULL;
	while(VirtualQueryEx(
		hProcess,
		baseAddr,
		&mbi,
		dwLen) != 0)
	{
		baseAddr = (char*)mbi.BaseAddress + mbi.RegionSize;

		TiXmlElement* section = new TiXmlElement("Memory");
		char szBase[128] = {0};
		sprintf(szBase, "0x%08X", (DWORD)mbi.BaseAddress);
		section->SetAttribute("BaseAddress", szBase);
		memset(szBase, 0, 128);
		sprintf(szBase, "%d pages", (DWORD)mbi.RegionSize / 4096);
		section->SetAttribute("RegionSize", szBase);
		section->SetAttribute("Type", TypeString(mbi.Type).c_str());
		section->SetAttribute("State", StateString(mbi.State).c_str());
		section->SetAttribute("Protection", ProtectString(mbi.Protect).c_str());
		process->LinkEndChild(section);
	}	
}

void MemoryRegionEnumerator::printParameters(PRTL_USER_PROCESS_PARAMETERS parameters,
	HANDLE hProcess,
	TiXmlElement* process)
{
	WCHAR wszCmdline[1024] = {0};
	DWORD dwLen = 1024 * sizeof(WCHAR);
	ReadProcessMemory(
		hProcess,
		parameters->CommandLine.Buffer,
		wszCmdline,
		dwLen,
		&dwLen);

	char szCmdline[1024] = {0};
	WideCharToMultiByte(
		CP_ACP,
		NULL,
		wszCmdline,
		dwLen,
		szCmdline,
		1024,
		NULL,
		NULL);
	process->SetAttribute("CommandLine", szCmdline);

	memset(szCmdline, 0, 1024);
	dwLen = 1024;
	ReadProcessMemory(
		hProcess,
		parameters->ImagePathName.Buffer,
		wszCmdline,
		dwLen,
		&dwLen);

	WideCharToMultiByte(
		CP_ACP,
		NULL,
		wszCmdline,
		dwLen,
		szCmdline,
		1024,
		NULL,
		NULL);

	process->SetAttribute("ImagePathName", szCmdline);
}

void MemoryRegionEnumerator::InitializeReceiver()
{
	m_doc = Report::GetDocument("memory_region.xml");
	if (m_doc == NULL)
	{
		m_bWork = false;
	}

	if (m_bWork)
	{
		m_root = new TiXmlElement("MemoryRegions");
	}
}

void MemoryRegionEnumerator::ReceivePid( DWORD dwPid )
{
	//printf("********************\nPID: %d\n", dwPid);
	char szPid[16] = {0};
	sprintf(szPid, "%d", dwPid);


	TiXmlElement* process = new TiXmlElement("process");

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

	typedef NTSTATUS (WINAPI * PFNNtQueryInformationProcess)(
		__in          HANDLE ProcessHandle,
		__in          PROCESSINFOCLASS ProcessInformationClass,
		__out         PVOID ProcessInformation,
		__in          ULONG ProcessInformationLength,
		__out_opt     PULONG ReturnLength
		);

	PFNNtQueryInformationProcess pfnNtQueryInformationProcess 
		= (PFNNtQueryInformationProcess)GetProcAddress(
		hNtdll,
		"NtQueryInformationProcess");

	PROCESS_BASIC_INFORMATION pbi;
	DWORD dwLen = sizeof(pbi);
	NTSTATUS ret = pfnNtQueryInformationProcess(
		hProcess,
		ProcessBasicInformation,
		&pbi,
		dwLen,
		&dwLen);
	if (!NT_SUCCESS(ret))
	{
		delete process;
		return;
	}

	PEB peb;
	memset(&peb, 0 , sizeof(peb));
	dwLen = sizeof(peb);
	ReadProcessMemory(
		hProcess,
		pbi.PebBaseAddress,
		&peb,
		dwLen,
		&dwLen);

	RTL_USER_PROCESS_PARAMETERS parameters;
	memset(&parameters, 0, sizeof(parameters));
	dwLen = sizeof(parameters);
	ReadProcessMemory(
		hProcess,
		peb.ProcessParameters,
		&parameters,
		dwLen,
		&dwLen);

	printParameters(&parameters, hProcess, process);
	printMemorySection(hProcess, process);

	m_root->LinkEndChild(process);
}

void MemoryRegionEnumerator::FinalizeReceiver()
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

MemoryRegionEnumerator::MemoryRegionEnumerator()
	:m_bWork(true)
{

}

MemoryRegionEnumerator::~MemoryRegionEnumerator()
{

}
