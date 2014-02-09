#include "PdhTest.h"
#include <windows.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
#include <stdio.h>
#include "report.h"
#include "tinyxml.h"
#include <WinPerf.h>

#define MAX_BUF 1024 * 2
#define INIT_SIZE 1024 * 100
#define ADD_SIZE 512 * 100

void printHelp(LPWSTR pBuffer, const char* file)
{
	TiXmlDocument* doc = Report::GetDocument(file);
	TiXmlElement* root = new TiXmlElement("helps");

	char szBuffer[MAX_BUF] = {0};
	LPWSTR pNext = NULL;

	while (wcslen(pBuffer) > 0)
	{
		TiXmlElement* node = new TiXmlElement("help");
		memset(szBuffer, 0, MAX_BUF);
		pNext = pBuffer + wcslen(pBuffer);
		if (WideCharToMultiByte(
			CP_ACP,
			NULL,
			pBuffer,
			wcslen(pBuffer),
			szBuffer,
			MAX_BUF,
			NULL,
			NULL
			) > 0)
		{
			printf("index: %s\n", szBuffer);
			node->SetAttribute("index", szBuffer);
		}
		
		pNext = pBuffer + wcslen(pBuffer) + 1;
		pBuffer = pNext;	

		memset(szBuffer, 0, MAX_BUF);
		pNext = pBuffer + wcslen(pBuffer);
		DWORD dwRet = WideCharToMultiByte(
			CP_ACP,
			NULL,
			pBuffer,
			wcslen(pBuffer),
			szBuffer,
			MAX_BUF,
			NULL,
			NULL
			);
		if ( dwRet > 0)
		{
			printf("value: %s\n", szBuffer);
			node->SetAttribute("value", szBuffer);
		}

		root->LinkEndChild(node);
		pNext = pBuffer + wcslen(pBuffer) + 1;
		pBuffer = pNext;
	}

	doc->LinkEndChild(root);
	doc->SaveFile();

	doc->Clear();
	delete doc;
	root = NULL;
	doc = NULL;
}

void PdhTest::testHelp()
{
	LPWSTR pBuffer = NULL;
	DWORD dwBufferSize = 0;
	LONG status = ERROR_SUCCESS;

	// Query the size of the text data so you can allocate the buffer.
	status = RegQueryValueEx(HKEY_PERFORMANCE_TEXT, L"Help", NULL, NULL, NULL, &dwBufferSize);
	if (ERROR_SUCCESS != status)
	{
		wprintf(L"RegQueryValueEx failed getting required buffer size. Error is 0x%x.\n", status);
		goto cleanup;
	}

	// Allocate the text buffer and query the text.
	pBuffer = (LPWSTR)malloc(dwBufferSize);
	if (pBuffer)
	{
		status = RegQueryValueEx(HKEY_PERFORMANCE_TEXT, L"help", NULL, NULL, (LPBYTE)pBuffer, &dwBufferSize);
		if (ERROR_SUCCESS != status)
		{
			wprintf(L"RegQueryValueEx failed with 0x%x.\n", status);
			free(pBuffer);
			pBuffer = NULL;
			goto cleanup;
		}
		printHelp(pBuffer, "PdhHelp.xml");
	}
	else
	{
		wprintf(L"malloc failed to allocate memory.\n");
	}

cleanup:

	return;
}

void PdhTest::testCounter()
{
	LPWSTR pBuffer = NULL;
	DWORD dwBufferSize = 0;
	LONG status = ERROR_SUCCESS;

	// Query the size of the text data so you can allocate the buffer.
// 	status = ERROR_MORE_DATA;
// 	while (ERROR_MORE_DATA == status)
// 	{
 		status = RegQueryValueEx(HKEY_PERFORMANCE_TEXT, L"Counter", NULL, NULL, NULL, &dwBufferSize);
// 	}

	if (ERROR_SUCCESS != status)
	{
		wprintf(L"RegQueryValueEx failed getting required buffer size. Error is 0x%x.\n", status);
		goto cleanup;
	}

	// Allocate the text buffer and query the text.
	pBuffer = (LPWSTR)malloc(dwBufferSize);
	if (pBuffer)
	{
		status = RegQueryValueEx(HKEY_PERFORMANCE_TEXT, L"Counter", NULL, NULL, (LPBYTE)pBuffer, &dwBufferSize);
		if (ERROR_SUCCESS != status)
		{
			wprintf(L"RegQueryValueEx failed with 0x%x.\n", status);
			free(pBuffer);
			pBuffer = NULL;
			goto cleanup;
		}
		printHelp(pBuffer, "PdhCounter.xml");
	}
	else
	{
		wprintf(L"malloc failed to allocate memory.\n");
	}

cleanup:

	return;
}

void PdhTest::test()
{
	PPERF_DATA_BLOCK pData = NULL;
	PPERF_OBJECT_TYPE pType = NULL;
	PPERF_INSTANCE_DEFINITION pInst = NULL;
	PPERF_COUNTER_DEFINITION pCounterDef = NULL;
	PPERF_COUNTER_BLOCK pCounter = NULL;

	LPBYTE pBuffer = NULL;
	DWORD dwBufferSize = 0;
	LONG status = ERROR_SUCCESS;
	DWORD dwType = 0;

	pBuffer = (LPBYTE)malloc(INIT_SIZE);
	dwBufferSize = INIT_SIZE;

	status = RegQueryValueEx(HKEY_PERFORMANCE_TEXT, L"26", NULL, &dwType, pBuffer, &dwBufferSize);

	if (ERROR_SUCCESS != status)
	{
		wprintf(L"RegQueryValueEx failed getting required buffer size. Error is 0x%x.\n", status);
		goto cleanup;
	}

	pData = (PPERF_DATA_BLOCK)pBuffer;
	pType = (PPERF_OBJECT_TYPE)((PBYTE)pData + pData->HeaderLength);
	pCounterDef = (PPERF_COUNTER_DEFINITION)((PBYTE)pType + pType->HeaderLength);
	pInst = (PPERF_INSTANCE_DEFINITION)((PBYTE)pType + pType->DefinitionLength);
	for (int i=0;i<pType->NumInstances;i++)
	{

		pInst = (PPERF_INSTANCE_DEFINITION)((PBYTE)pInst + pInst->ByteLength);
	}

cleanup:

	return;
}

PdhTest::PdhTest()
{

}

PdhTest::~PdhTest()
{

}
