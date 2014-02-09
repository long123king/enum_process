#include "thThreadEnumerator.h"
#include <TlHelp32.h>

void printThread(PTHREADENTRY32 entry, TiXmlElement* process)
{
	TiXmlElement* node = new TiXmlElement("thread");

	TiXmlElement* item = new TiXmlElement("ThreadID");
	item->SetAttribute("value", (DWORD)entry->th32ThreadID);
	node->LinkEndChild(item);

	item = new TiXmlElement("Priority");
	item->SetAttribute("value", entry->tpBasePri);
	node->LinkEndChild(item);

	process->LinkEndChild(node);
}

void thThreadEnumerator::InitializeReceiver()
{
	m_doc = Report::GetDocument("thThread.xml");
	m_root = new TiXmlElement("threads");
}

void thThreadEnumerator::ReceivePid( DWORD dwPid )
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPTHREAD,
		dwPid);

	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//m_bWork = FALSE;
		return;
	}

	THREADENTRY32 entry;
	memset(&entry, 0, sizeof(entry));
	entry.dwSize = sizeof(entry);

	TiXmlElement* node = new TiXmlElement("process");
	node->SetAttribute("PID", dwPid);

	BOOL bRet = Thread32First(
		hSnapshot,
		&entry);

	while (bRet)
	{
		if (entry.th32OwnerProcessID == dwPid)
		{
			printThread(&entry, node);
		}	

		memset(&entry, 0, sizeof(entry));
		entry.dwSize = sizeof(entry);
		bRet = Thread32Next(
			hSnapshot,
			&entry);		
	}

	m_root->LinkEndChild(node);

	CloseHandle(hSnapshot);
}

void thThreadEnumerator::FinalizeReceiver()
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


thThreadEnumerator::thThreadEnumerator()
	:m_bWork(TRUE)
{

}

thThreadEnumerator::~thThreadEnumerator()
{

}

