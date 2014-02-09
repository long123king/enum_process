#include "DllEnumerator.h"

#include <Psapi.h>
#pragma comment(lib, "psapi.lib")


#define CONTAINING_OF(address, type, field) ((type *)(	\
	(char*)(address) -	\
	(ULONG_PTR)(&((type *)0)->field)))  

typedef struct _LDR_DATA_TABLE_ENTRY1 {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		struct {
			ULONG TimeDateStamp;
		};
		struct {
			PVOID LoadedImports;
		};
	};
	//struct _ACTIVATION_CONTEXT * EntryPointActivationContext;
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY1, *PLDR_DATA_TABLE_ENTRY1;

void DllEnumerator::printPEB(PPEB ppeb, TiXmlElement* process)
{
// 	printf("PEB:\n");
// 	printf("\tBeingDebugged: %d\n", ppeb->BeingDebugged);
// 	printf("\tSessionId: %d\n", ppeb->SessionId);
}

void DllEnumerator::printLdr(PPEB_LDR_DATA ldr, HANDLE hProcess, TiXmlElement* process)
{
	// 	printf("Ldr:\n");
	// 	printf("\tFlink: 0x%08X\n", ldr->InMemoryOrderModuleList.Flink);
	// 	printf("\tBlink: 0x%08X\n", ldr->InMemoryOrderModuleList.Blink);

	LIST_ENTRY* head = ldr->InMemoryOrderModuleList.Blink;
	LIST_ENTRY* tag = ldr->InMemoryOrderModuleList.Flink;

	int i = 0;
	do
	{
		LDR_DATA_TABLE_ENTRY1 entry;
		DWORD dwLen = sizeof(entry);
		ReadProcessMemory(
			hProcess,
			CONTAINING_OF(tag, LDR_DATA_TABLE_ENTRY1, InMemoryOrderLinks),
			&entry,
			dwLen,
			&dwLen);

		WCHAR FullDllName[1024] = {0};
		dwLen = 1024 * sizeof(WCHAR);
		ReadProcessMemory(
			hProcess,
			entry.FullDllName.Buffer,
			&FullDllName,
			dwLen,
			&dwLen);

		char szFullName[1024] = {0};
		WideCharToMultiByte(
			CP_ACP,
			NULL,
			FullDllName,
			dwLen,
			szFullName,
			1024,
			NULL,
			NULL);

		WCHAR BaseDllName[1024] = {0};
		dwLen = 1024 * sizeof(WCHAR);
		ReadProcessMemory(
			hProcess,
			entry.BaseDllName.Buffer,
			&BaseDllName,
			dwLen,
			&dwLen);

		char szBaseName[1024] = {0};
		WideCharToMultiByte(
			CP_ACP,
			NULL,
			BaseDllName,
			dwLen,
			szBaseName,
			1024,
			NULL,
			NULL);

		char base[128] = {0};
		sprintf(base, "0x%08X", entry.DllBase);

		TiXmlElement* moduleNode = new TiXmlElement("DLL");

		

		TiXmlElement* node = new TiXmlElement("FullDllName");
		node->SetAttribute("value", szFullName);
		moduleNode->LinkEndChild(node);

// 		node = new TiXmlElement("BaseDllName");
// 		node->SetAttribute("value", szBaseName);
// 		moduleNode->LinkEndChild(node);
		moduleNode->SetAttribute("name", szBaseName);

		node = new TiXmlElement("BaseAddress");
		node->SetAttribute("value", base);
		moduleNode->LinkEndChild(node);


		memset(base, 0, 128);
		sprintf(base, "0x%08X", entry.EntryPoint);

		node = new TiXmlElement("SizeOfImage");
		node->SetAttribute("value", entry.SizeOfImage);
		moduleNode->LinkEndChild(node);

		node = new TiXmlElement("EntryPoint");
		node->SetAttribute("value", base);
		moduleNode->LinkEndChild(node);

		//moduleNode->SetValue(module);
		process->LinkEndChild(moduleNode);
		//delete moduleNode;
		//Report::GetInstance()->AddAttribute(process, attr, module);

		// 		printf("Checksum: %d\n", entry.CheckSum);
		// 		printf("Time&Date stamp: %d\n", entry.TimeDateStamp);

		tag = entry.InMemoryOrderLinks.Flink;
	}while(head != tag);

}

void DllEnumerator::printParameters(PRTL_USER_PROCESS_PARAMETERS parameters,
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

void DllEnumerator::InitializeReceiver()
{
	m_doc = Report::GetDocument("dll.xml");
	if (m_doc == NULL)
	{
		m_bWork = false;
	}

	if (m_bWork)
	{
		m_root = new TiXmlElement("LoadedModules");
	}
}

void DllEnumerator::ReceivePid( DWORD dwPid )
{
//	printf("********************\nPID: %d\n", dwPid);
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

	// 		UNICODE_STRING imageName;
	// 		dwLen = sizeof(imageName);
	// 		ret = pfnNtQueryInformationProcess(
	// 			hProcess,
	// 			ProcessImageFileName,
	// 			&imageName,
	// 			dwLen,
	// 			&dwLen);
	// 		if (!NT_SUCCESS(ret))
	// 		{
	// 			continue;
	// 		}

	PEB peb;
	memset(&peb, 0 , sizeof(peb));
	dwLen = sizeof(peb);
	ReadProcessMemory(
		hProcess,
		pbi.PebBaseAddress,
		&peb,
		dwLen,
		&dwLen);

	printPEB(&peb, process);

	PEB_LDR_DATA ldr;
	memset(&ldr, 0, sizeof(ldr));
	dwLen = sizeof(ldr);
	ReadProcessMemory(
		hProcess,
		peb.Ldr,
		&ldr,
		dwLen,
		&dwLen);

	printLdr(&ldr, hProcess, process);

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

	m_root->LinkEndChild(process);
}

void DllEnumerator::FinalizeReceiver()
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

DllEnumerator::DllEnumerator()
	:m_bWork(true)
{

}

DllEnumerator::~DllEnumerator()
{

}
