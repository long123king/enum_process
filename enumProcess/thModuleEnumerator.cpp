#include "thModuleEnumerator.h"
#include <TlHelp32.h>

void printModule(PMODULEENTRY32 entry, TiXmlElement* process)
{
    TiXmlElement* node = new TiXmlElement("module");

	TiXmlElement* item = new TiXmlElement("HANDLE");
	item->SetAttribute("value", (DWORD)entry->hModule);
	node->LinkEndChild(item);

	item = new TiXmlElement("Address");
	char szBuffer[1024] = {0};
	sprintf(szBuffer, "0x%08X", entry->modBaseAddr);
	item->SetAttribute("value", szBuffer);
	node->LinkEndChild(item);

	item = new TiXmlElement("Size");
	item->SetAttribute("value", entry->modBaseSize);
	node->LinkEndChild(item);

	item = new TiXmlElement("Name");
	memset(szBuffer, 0, 1024);
	if (WideCharToMultiByte(
		CP_ACP,
		NULL,
		entry->szModule,
		wcslen(entry->szModule),
		szBuffer,
		1024,
		NULL,
		NULL) > 0)
	{
		item->SetAttribute("value", szBuffer);
		node->LinkEndChild(item);
	}
	else
		delete item;

	item = new TiXmlElement("Path");
	memset(szBuffer, 0, 1024);
	if (WideCharToMultiByte(
		CP_ACP,
		NULL,
		entry->szExePath,
		wcslen(entry->szExePath),
		szBuffer,
		1024,
		NULL,
		NULL) > 0)
	{
		item->SetAttribute("value", szBuffer);
		node->LinkEndChild(item);
	}
	else
		delete item;

	process->LinkEndChild(node);
}

void thModuleEnumerator::InitializeReceiver()
{
	m_doc = Report::GetDocument("thModule.xml");
	m_root = new TiXmlElement("modules");
}

void thModuleEnumerator::ReceivePid( DWORD dwPid )
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPMODULE,
		dwPid);

	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//m_bWork = FALSE;
		return;
	}

	MODULEENTRY32 entry;
	memset(&entry, 0, sizeof(entry));
	entry.dwSize = sizeof(entry);

	TiXmlElement* node = new TiXmlElement("process");
	node->SetAttribute("PID", dwPid);

	BOOL bRet = Module32First(
		hSnapshot,
		&entry);

	while (bRet)
	{
		printModule(&entry, node);

		memset(&entry, 0, sizeof(entry));
		entry.dwSize = sizeof(entry);
		bRet = Module32Next(
			hSnapshot,
			&entry);		
	}

	m_root->LinkEndChild(node);

	CloseHandle(hSnapshot);
}

void thModuleEnumerator::FinalizeReceiver()
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

thModuleEnumerator::thModuleEnumerator()
	:m_bWork(TRUE)
{

}

thModuleEnumerator::~thModuleEnumerator()
{

}

