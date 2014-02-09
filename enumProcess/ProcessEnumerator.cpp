#include "ProcessEnumerator.h"
#include <winternl.h>   // for Windows internal declarations.
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

#include "Log.h"

ProcessEnumerator::ProcessEnumerator()
{
	m_vecReceivers.clear();
}

ProcessEnumerator::~ProcessEnumerator()
{
	m_vecReceivers.clear();
}

void ProcessEnumerator::AddReceiver( IPidReceiver* receiver )
{
	m_vecReceivers.push_back(receiver);
}

void ProcessEnumerator::Enumerate()
{
	DWORD *pBuffer = new DWORD[1024];
	if (pBuffer == NULL)
	{
		return;
	}

	DWORD dwSize = 1024 * sizeof(DWORD);
	BOOL bResult = EnumProcesses(pBuffer, dwSize, &dwSize);
	if (!bResult || dwSize > 1024 * sizeof(DWORD))
	{
		delete[] pBuffer;
		return;
	}
	DWORD dwNums = dwSize / sizeof(DWORD);
	//printf("Numbers: %d\n", dwNums);

	InitializeReceivers();
	for (int i=0;i<dwNums;i++)
	{
		DWORD dwPID = pBuffer[i];
		LOG("processing %5d......\n", dwPID);
		SendPid2Receivers(dwPID);

	}
	FinalizeReceivers();
	delete[] pBuffer;
}

void ProcessEnumerator::InitializeReceivers()
{
	for (int i=0;i<m_vecReceivers.size();i++)
	{
		IPidReceiver* receiver = m_vecReceivers.at(i);
		receiver->InitializeReceiver();
	}
}

void ProcessEnumerator::SendPid2Receivers( DWORD dwPid )
{
	for (int i=0;i<m_vecReceivers.size();i++)
	{
		IPidReceiver* receiver = m_vecReceivers.at(i);
		receiver->ReceivePid(dwPid);
	}
}

void ProcessEnumerator::FinalizeReceivers()
{
	for (int i=0;i<m_vecReceivers.size();i++)
	{
		IPidReceiver* receiver = m_vecReceivers.at(i);
		receiver->FinalizeReceiver();
	}
}
