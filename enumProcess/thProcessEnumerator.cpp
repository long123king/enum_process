#include "thProcessEnumerator.h"
#include <winternl.h>   // for Windows internal declarations.
#include <TlHelp32.h>

void printProcess(PPROCESSENTRY32 entry, TiXmlElement* parent)
{
	TiXmlElement* process = new TiXmlElement("process");
	process->SetAttribute("PID", entry->th32ProcessID);
	
	TiXmlElement* node = new TiXmlElement("ThreadCounts");
	node->SetAttribute("value", entry->cntThreads);
	process->LinkEndChild(node);

	node = new TiXmlElement("ParentPid");
	node->SetAttribute("value", entry->th32ParentProcessID);
	process->LinkEndChild(node);

	node = new TiXmlElement("BasePriority");
	node->SetAttribute("value", entry->pcPriClassBase);
	process->LinkEndChild(node);

	node = new TiXmlElement("ExeFile");
	char szName[2048] = {0};
	if (WideCharToMultiByte(
		CP_ACP,
		NULL,
		entry->szExeFile,
		wcslen(entry->szExeFile),
		szName,
		2048,
		NULL,
		NULL) > 0)
	{
		node->SetAttribute("value", szName);
		process->LinkEndChild(node);
	}
	else
	{
		delete node;
	}

	parent->LinkEndChild(process);
}

thProcessEnumerator::thProcessEnumerator()
	:m_bWork(TRUE)
{
	m_vecReceivers.clear();
	
}

thProcessEnumerator::~thProcessEnumerator()
{
	m_vecReceivers.clear();
	
}

void thProcessEnumerator::AddReceiver( IPidReceiver* receiver )
{
	if (m_bWork)
		m_vecReceivers.push_back(receiver);
}

void thProcessEnumerator::Enumerate()
{
	m_hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPPROCESS,
		0);

	if (INVALID_HANDLE_VALUE == m_hSnapshot)
	{
		m_bWork = FALSE;
		return;
	}

	m_doc = Report::GetDocument("thProcesses.xml");
	m_root = new TiXmlElement("processes");
	PROCESSENTRY32 entry;
	memset(&entry, 0, sizeof(entry));
	entry.dwSize = sizeof(entry);
	
	InitializeReceivers();
	BOOL bRet = Process32First(
		m_hSnapshot,
		&entry);

	while(bRet)
	{
		printProcess(&entry, m_root);
		DWORD dwPID = entry.th32ProcessID;
		SendPid2Receivers(dwPID);
		bRet = Process32Next(
			m_hSnapshot,
			&entry);
	}

	FinalizeReceivers();

	m_doc->LinkEndChild(m_root);
	m_doc->SaveFile();

	m_doc->Clear();
	delete m_doc;
	m_root = NULL;
	m_doc = NULL;

	CloseHandle(m_hSnapshot);
	m_hSnapshot = NULL;
}

void thProcessEnumerator::InitializeReceivers()
{
	for (int i=0;i<m_vecReceivers.size();i++)
	{
		IPidReceiver* receiver = m_vecReceivers.at(i);
		receiver->InitializeReceiver();
	}
}

void thProcessEnumerator::SendPid2Receivers( DWORD dwPid )
{
	for (int i=0;i<m_vecReceivers.size();i++)
	{
		IPidReceiver* receiver = m_vecReceivers.at(i);
		receiver->ReceivePid(dwPid);
	}
}

void thProcessEnumerator::FinalizeReceivers()
{
	for (int i=0;i<m_vecReceivers.size();i++)
	{
		IPidReceiver* receiver = m_vecReceivers.at(i);
		receiver->FinalizeReceiver();
	}
}
