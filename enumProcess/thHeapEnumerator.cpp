#include "thHeapEnumerator.h"
#include <TlHelp32.h>

std::string FlagString(DWORD value)
{
	std::string result;
	if (value == LF32_FIXED)
	{
		result = "FIXED";
	} 
	else if(value == LF32_FREE)
	{
		result = "FREE";
	}
	else if(value == LF32_MOVEABLE)
	{
		result = "MOVEABLE";
	}
	else
	{
		result = "";
	}
	return result;
}

void printHeaps(DWORD thProcessId, ULONG_PTR thHeapId, TiXmlElement* process)
{
	HEAPENTRY32 entry;
	memset(&entry, 0, sizeof(entry));
	entry.dwSize = sizeof(entry);

	BOOL bRet = Heap32First(
		&entry,
		thProcessId,
		thHeapId);

	while (bRet)
	{	
		TiXmlElement* node = new TiXmlElement("Heap");

		TiXmlElement* item = new TiXmlElement("HANDLE");
		item->SetAttribute("value", (DWORD)entry.hHandle);
		node->LinkEndChild(item);
		
		item = new TiXmlElement("Heap ID");
		item->SetAttribute("value", entry.th32HeapID);
		node->LinkEndChild(item);

		item = new TiXmlElement("Address");
		char szBuffer[1024] = {0};
		sprintf(szBuffer, "0x%08X", entry.dwAddress);
		item->SetAttribute("value", szBuffer);
		node->LinkEndChild(item);

		item = new TiXmlElement("BlockSize");
		item->SetAttribute("value", entry.dwBlockSize);
		node->LinkEndChild(item);

		item = new TiXmlElement("LockCount");
		item->SetAttribute("value", entry.dwLockCount);
		node->LinkEndChild(item);
		
// 		item = new TiXmlElement("PID");
// 		item->SetAttribute("value", entry.th32ProcessID);
// 		node->LinkEndChild(item);

		item = new TiXmlElement("Reserved");
		item->SetAttribute("value", entry.dwResvd);
		node->LinkEndChild(item);

		item = new TiXmlElement("Flag");
		item->SetAttribute("value", FlagString(entry.dwFlags).c_str());
		node->LinkEndChild(item);

		process->LinkEndChild(node);

		memset(&entry, 0, sizeof(entry));
		entry.dwSize = sizeof(entry);

		bRet = Heap32Next(&entry);
	}
}

void thHeapEnumerator::InitializeReceiver()
{
	m_doc = Report::GetDocument("heap.xml");
	m_root = new TiXmlElement("heaps");
}

void thHeapEnumerator::ReceivePid(DWORD dwPid)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(
		TH32CS_SNAPHEAPLIST,
		dwPid);

	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		//m_bWork = FALSE;
		return;
	}

	HEAPLIST32 heaplist;
	memset(&heaplist, 0, sizeof(heaplist));
	heaplist.dwSize = sizeof(heaplist);

	TiXmlElement* node = new TiXmlElement("heaplist");
	node->SetAttribute("PID", dwPid);

	BOOL bRet = Heap32ListFirst(
		hSnapshot,
		&heaplist);

	while (bRet)
	{
		printHeaps(heaplist.th32ProcessID, heaplist.th32HeapID, node);

		memset(&heaplist, 0, sizeof(heaplist));
		heaplist.dwSize = sizeof(heaplist);
		bRet = Heap32ListNext(
			hSnapshot,
			&heaplist);		
	}

	m_root->LinkEndChild(node);

	CloseHandle(hSnapshot);

}

void thHeapEnumerator::FinalizeReceiver()
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

thHeapEnumerator::thHeapEnumerator()
	:m_bWork(TRUE)
{

}

thHeapEnumerator::~thHeapEnumerator()
{

}

