#include "pdhProcessEnumerator.h"
#define WIN32_LEAN_AND_MEAN 1

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <vector>
#include <string>
#include <map>

#include "sqlite3.h"

#pragma comment(lib, "pdh.lib")


void enumObjIns(LPCWSTR counterObj, std::vector<std::string>& vecObj, std::vector<std::string>& vecIns)
{
	PDH_STATUS status = ERROR_SUCCESS;
	LPWSTR pwsCounterListBuffer = NULL;
	DWORD dwCounterListSize = 0;
	LPWSTR pwsInstanceListBuffer = NULL;
	DWORD dwInstanceListSize = 0;
	LPWSTR pTemp = NULL;

	char szBuffer[1024] = {0};

	// Determine the required buffer size for the data. 
	status = PdhEnumObjectItems(
		NULL,                   // real-time source
		NULL,                   // local machine
		counterObj,         // object to enumerate
		pwsCounterListBuffer,   // pass NULL and 0
		&dwCounterListSize,     // to get required buffer size
		pwsInstanceListBuffer, 
		&dwInstanceListSize, 
		PERF_DETAIL_WIZARD,     // counter detail level
		0); 

	if (status == PDH_MORE_DATA) 
	{
		// Allocate the buffers and try the call again.
		pwsCounterListBuffer = (LPWSTR)malloc(dwCounterListSize * sizeof(WCHAR));
		pwsInstanceListBuffer = (LPWSTR)malloc(dwInstanceListSize * sizeof(WCHAR));

		if (NULL != pwsCounterListBuffer && NULL != pwsInstanceListBuffer) 
		{
			status = PdhEnumObjectItems(
				NULL,                   // real-time source
				NULL,                   // local machine
				counterObj,         // object to enumerate
				pwsCounterListBuffer, 
				&dwCounterListSize,
				pwsInstanceListBuffer, 
				&dwInstanceListSize, 
				PERF_DETAIL_WIZARD,     // counter detail level
				0); 

			if (status == ERROR_SUCCESS) 
			{
				//wprintf(L"Counters that the Process objects defines:\n\n");

				// Walk the counters list. The list can contain one
				// or more null-terminated strings. The list is terminated
				// using two null-terminator characters.
				for (pTemp = pwsCounterListBuffer; *pTemp != 0; pTemp += wcslen(pTemp) + 1) 
				{
					//wprintf(L"%s\n", pTemp);
					WideCharToMultiByte(
						CP_ACP,
						NULL,
						pTemp,
						wcslen(pTemp),
						szBuffer,
						1024,
						NULL,
						NULL);
					szBuffer[wcslen(pTemp)] = '\0';
					vecObj.push_back(szBuffer);
				}

				//wprintf(L"\nInstances of the Process object:\n\n");

				// Walk the instance list. The list can contain one
				// or more null-terminated strings. The list is terminated
				// using two null-terminator characters.
				for (pTemp = pwsInstanceListBuffer; *pTemp != 0; pTemp += wcslen(pTemp) + 1) 
				{
					//wprintf(L"%s\n", pTemp);
					WideCharToMultiByte(
						CP_ACP,
						NULL,
						pTemp,
						wcslen(pTemp),
						szBuffer,
						1024,
						NULL,
						NULL);
					szBuffer[wcslen(pTemp)] = '\0';
					vecIns.push_back(szBuffer);
				}
			}
			else 
			{
				//wprintf(L"Second PdhEnumObjectItems failed with %0x%x.\n", status);
			}
		} 
		else 
		{
			//wprintf(L"Unable to allocate buffers.\n");
			status = ERROR_OUTOFMEMORY;
		}
	} 
	else 
	{
		//wprintf(L"\nPdhEnumObjectItems failed with 0x%x.\n", status);
	}

	if (pwsCounterListBuffer != NULL) 
		free (pwsCounterListBuffer);

	if (pwsInstanceListBuffer != NULL) 
		free (pwsInstanceListBuffer);
}

BOOL getCounter(LPCWSTR counterName, 
	int timeInterval, 
	PDH_RAW_COUNTER& counter)
{
	HQUERY hQuery;
	if(PdhOpenQuery(NULL, NULL, &hQuery) != ERROR_SUCCESS)
		return FALSE;

	PDH_HCOUNTER pCounter;
	if (ERROR_SUCCESS != PdhAddCounter(hQuery,
		counterName,
		NULL,
		&pCounter))
	{
		return FALSE;
	}

	PdhCollectQueryData(hQuery);

	if (timeInterval > 0)
		Sleep(timeInterval);

	DWORD dwTmp = 0;
	PdhGetRawCounterValue(pCounter,
		&dwTmp,
		&counter);	

	return TRUE;
}

BOOL getCounter(LPCWSTR counterName, 
	int timeInterval, 
	PDH_RAW_COUNTER& counter, 
	DWORD& dwType)
{
	HQUERY hQuery;
	if(PdhOpenQuery(NULL, NULL, &hQuery) != ERROR_SUCCESS)
		return FALSE;

	PDH_HCOUNTER pCounter;
	if (ERROR_SUCCESS != PdhAddCounter(hQuery,
		counterName,
		NULL,
		&pCounter))
	{
		return FALSE;
	}

	PdhCollectQueryData(hQuery);

	if (timeInterval > 0)
		Sleep(timeInterval);

	DWORD dwTmp = 0;
	PdhGetRawCounterValue(pCounter,
		&dwTmp,
		&counter);	

	dwType = dwTmp;

	return TRUE;
}

BOOL getCounter(
	LPCWSTR counterName, 
	int timeInterval, 
	PDH_RAW_COUNTER& counter, 
	DWORD& dwType, 
	LONGLONG& pTimebase)
{
	HQUERY hQuery;
	if(PdhOpenQuery(NULL, NULL, &hQuery) != ERROR_SUCCESS)
		return FALSE;

	PDH_HCOUNTER pCounter;
	if (ERROR_SUCCESS != PdhAddCounter(hQuery,
		counterName,
		NULL,
		&pCounter))
	{
		return FALSE;
	}

	PdhCollectQueryData(hQuery);

	if (timeInterval > 0)
		Sleep(timeInterval);

	DWORD dwTmp = 0;
	PdhGetRawCounterValue(pCounter,
		&dwTmp,
		&counter);	

	dwType = dwTmp;

	LONGLONG llTimebase = 0;
	PdhGetCounterTimeBase(pCounter,
		&llTimebase);

	pTimebase = llTimebase;

	return TRUE;
}

std::string getCounterTimestamp(FILETIME* ft)
{
	SYSTEMTIME st;
	BOOL bRet = FileTimeToSystemTime(
		ft,
		&st);
	if (!bRet)
	{
		return NULL;
	}

	char szTime[1024] = {0};
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond);
	return szTime;
}

std::string getTimeInterval(FILETIME* ft1, FILETIME* ft2)
{
	__int64 llInterval = *(__int64*)ft2 - *(__int64*)ft1;
	__int64 llSeconds = llInterval / (__int64)10000000;

	char szTime[1024] = {0};
	DWORD dwSeconds = 0;
	DWORD dwMinutes = 0;
	DWORD dwHours = 0;
	DWORD dwDays = 0;
	
	dwSeconds = llSeconds % 60;
	dwMinutes = llSeconds / 60 % 60;
	dwHours = llSeconds / 60 / 60 % 24;
	dwDays = llSeconds / 60 / 60 / 24;

	if (dwDays == 0)
	{
		sprintf(szTime,
			"%2d:%2d:%2d",
			dwHours,
			dwMinutes,
			dwSeconds);
	} 
	else
	{
		sprintf(szTime,
			"%d days, %2d:%2d:%2d",
			dwDays,
			dwHours,
			dwMinutes,
			dwSeconds);
	}

	

	return szTime;
}


std::string getCounterValue(PDH_RAW_COUNTER counter1, PDH_RAW_COUNTER counter2, DWORD dwType, LONGLONG llTimebase)
{
	std::string strValue;

	LONGLONG llDiv = 0;
	LONGLONG llMul = 0;
	LONGLONG llVal = 0;

	char szBuffer[1024] = {0};
	switch (dwType) 
	{
	case PERF_COUNTER_COUNTER:  //(N1 - N0)/((D1 - D0)/F)
	case PERF_SAMPLE_COUNTER:
	case PERF_COUNTER_BULK_COUNT:  
		llDiv = counter2.SecondValue - counter1.SecondValue;
		llMul = counter2.FirstValue - counter1.FirstValue;
		if (llDiv == 0)
		{
			sprintf(szBuffer, "0");
			break;
		}
		llVal = llTimebase * llMul / llDiv;
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_COUNTER_QUEUELEN_TYPE:  //(N1 - N0)/(D1 - D0)
	case PERF_COUNTER_100NS_QUEUELEN_TYPE:  
	case PERF_COUNTER_OBJ_TIME_QUEUELEN_TYPE:
	case PERF_COUNTER_LARGE_QUEUELEN_TYPE:  
	case PERF_AVERAGE_BULK:  //don't display
		llDiv = counter2.SecondValue - counter1.SecondValue;
		llMul = counter2.FirstValue - counter1.FirstValue;
		if (llDiv == 0)
		{
			sprintf(szBuffer, "0");
			break;
		}
		llVal = llMul / llDiv;
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_OBJ_TIME_TIMER:  // 100*(N1 - N0)/(D1 - D0)
	case PERF_COUNTER_TIMER:  
	case PERF_100NSEC_TIMER:
	case PERF_PRECISION_SYSTEM_TIMER: 
	case PERF_PRECISION_100NS_TIMER:
	case PERF_PRECISION_OBJECT_TIMER:
	case PERF_SAMPLE_FRACTION:  // 100*(N1 - N0)/(B1 - B0)
		llDiv = counter2.SecondValue - counter1.SecondValue;
		llMul = counter2.FirstValue - counter1.FirstValue;
		if (llDiv == 0)
		{
			sprintf(szBuffer, "0");
			break;
		}
		llVal = 100 * llMul / llDiv;
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_COUNTER_TIMER_INV:  // 100*(1- ((N1 - N0)/(D1 - D0)))
	case PERF_100NSEC_TIMER_INV:  
		llDiv = counter2.SecondValue - counter1.SecondValue;
		llMul = counter2.FirstValue - counter1.FirstValue;
		if (llDiv == 0)
		{
			sprintf(szBuffer, "0");
			break;
		}
		llVal = 100*(1 - ((double)llMul/llDiv));
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_COUNTER_MULTI_TIMER:  // 100*((N1 - N0)/((D1 - D0)/TB))/B1
		llMul = counter2.FirstValue - counter1.FirstValue;
		llDiv = counter2.SecondValue - counter1.SecondValue;
		llDiv /= llTimebase; // Caution: here may be wrong, should be counter2.llTimebase
		if (llDiv == 0)
		{
			sprintf(szBuffer, "0");
			break;
		}
		llVal = 100*((double)llMul/llDiv)/counter2.MultiCount;
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_100NSEC_MULTI_TIMER:  // 100*((N1 - N0)/(D1 - D0))/B1
		llMul = counter2.FirstValue - counter1.FirstValue;
		llDiv = counter2.SecondValue - counter1.SecondValue;
		if (llDiv == 0)
		{
			sprintf(szBuffer, "0");
			break;
		}
		llVal = 100*((double)llMul/llDiv)/counter2.MultiCount;
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_COUNTER_MULTI_TIMER_INV:  // 100*(B1- ((N1 - N0)/(D1 - D0)))
	case PERF_100NSEC_MULTI_TIMER_INV:
		llMul = counter2.FirstValue - counter1.FirstValue;
		llDiv = counter2.SecondValue - counter1.SecondValue;
		if (llDiv == 0)
		{
			sprintf(szBuffer, "0");
			break;
		}
		llVal = 100*(counter2.MultiCount - ((double)llMul/llDiv));
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_COUNTER_RAWCOUNT:  // N as decimal
	case PERF_COUNTER_LARGE_RAWCOUNT:
		llVal = counter1.FirstValue;
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_COUNTER_RAWCOUNT_HEX:  // N as hexadecimal
	case PERF_COUNTER_LARGE_RAWCOUNT_HEX:
		llVal = counter1.FirstValue;
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_COUNTER_DELTA:  // N1 - N0
	case PERF_COUNTER_LARGE_DELTA:
		llVal = (double)(counter2.FirstValue - counter1.FirstValue);
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_RAW_FRACTION:  // 100*N/B
	case PERF_LARGE_RAW_FRACTION:
		llVal = (double)100*counter1.FirstValue/counter1.SecondValue;
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_AVERAGE_TIMER:  // ((N1 - N0)/TB)/(B1 - B0)
		llMul = counter2.FirstValue - counter1.FirstValue;
		llDiv = counter2.SecondValue - counter1.SecondValue;
		if (llDiv == 0)
		{
			sprintf(szBuffer, "0");
			break;
		}
		llVal = (double)llMul/llTimebase/llDiv;// Caution: here may be wrong, should be counter2.llTimebase
		sprintf(szBuffer, "%d", llVal);
		break;

	case PERF_ELAPSED_TIME:  //(D0 - N0)/F
		llVal = (double)(counter1.SecondValue - counter1.FirstValue)/llTimebase;// Caution: should be counter1.llTimebase
		sprintf(szBuffer, "%s", Report::GetTextTime(llVal * 1000).c_str());
		break;

	default:
		sprintf(szBuffer, "0");
		break;
	}
	strValue = szBuffer;
	return strValue;
}

void SavePerfTypes()
{
	DWORD dwConstant[] = {
		PERF_COUNTER_COUNTER,
		PERF_COUNTER_QUEUELEN_TYPE,
		PERF_SAMPLE_COUNTER,
		PERF_OBJ_TIME_TIMER,
		PERF_COUNTER_100NS_QUEUELEN_TYPE,
		PERF_COUNTER_OBJ_TIME_QUEUELEN_TYPE,
		PERF_COUNTER_TIMER,
		PERF_COUNTER_TIMER_INV,
		PERF_COUNTER_BULK_COUNT,
		PERF_COUNTER_LARGE_QUEUELEN_TYPE,
		PERF_COUNTER_MULTI_TIMER,
		PERF_COUNTER_MULTI_TIMER_INV,
		PERF_COUNTER_RAWCOUNT,
		PERF_COUNTER_RAWCOUNT_HEX,
		PERF_COUNTER_DELTA,
		PERF_COUNTER_LARGE_RAWCOUNT,
		PERF_COUNTER_LARGE_RAWCOUNT_HEX,
		PERF_COUNTER_LARGE_DELTA,
		PERF_100NSEC_TIMER,
		PERF_100NSEC_TIMER_INV,
		PERF_100NSEC_MULTI_TIMER,
		PERF_100NSEC_MULTI_TIMER_INV,
		PERF_SAMPLE_FRACTION,
		PERF_RAW_FRACTION,
		PERF_LARGE_RAW_FRACTION,
		PERF_PRECISION_SYSTEM_TIMER,
		PERF_PRECISION_100NS_TIMER,
		PERF_PRECISION_OBJECT_TIMER,
		PERF_AVERAGE_TIMER,
		PERF_AVERAGE_BULK,
		PERF_SAMPLE_BASE,
		PERF_AVERAGE_BASE,
		PERF_COUNTER_MULTI_BASE,
		PERF_RAW_BASE,
		PERF_LARGE_RAW_BASE,
		PERF_ELAPSED_TIME,
		PERF_COUNTER_TEXT,
		PERF_COUNTER_NODATA,
		PERF_COUNTER_HISTOGRAM_TYPE
	};

	char* szDescription[] = {
		"PERF_COUNTER_COUNTER",
		"PERF_COUNTER_QUEUELEN_TYPE",
		"PERF_SAMPLE_COUNTER",
		"PERF_OBJ_TIME_TIMER",
		"PERF_COUNTER_100NS_QUEUELEN_TYPE",
		"PERF_COUNTER_OBJ_TIME_QUEUELEN_TYPE",
		"PERF_COUNTER_TIMER",
		"PERF_COUNTER_TIMER_INV",
		"PERF_COUNTER_BULK_COUNT",
		"PERF_COUNTER_LARGE_QUEUELEN_TYPE",
		"PERF_COUNTER_MULTI_TIMER",
		"PERF_COUNTER_MULTI_TIMER_INV",
		"PERF_COUNTER_RAWCOUNT",
		"PERF_COUNTER_RAWCOUNT_HEX",
		"PERF_COUNTER_DELTA",
		"PERF_COUNTER_LARGE_RAWCOUNT",
		"PERF_COUNTER_LARGE_RAWCOUNT_HEX",
		"PERF_COUNTER_LARGE_DELTA",
		"PERF_100NSEC_TIMER",
		"PERF_100NSEC_TIMER_INV",
		"PERF_100NSEC_MULTI_TIMER",
		"PERF_100NSEC_MULTI_TIMER_INV",
		"PERF_SAMPLE_FRACTION",
		"PERF_RAW_FRACTION",
		"PERF_LARGE_RAW_FRACTION",
		"PERF_PRECISION_SYSTEM_TIMER",
		"PERF_PRECISION_100NS_TIMER",
		"PERF_PRECISION_OBJECT_TIMER",
		"PERF_AVERAGE_TIMER",
		"PERF_AVERAGE_BULK",
		"PERF_SAMPLE_BASE",
		"PERF_AVERAGE_BASE",
		"PERF_COUNTER_MULTI_BASE",
		"PERF_RAW_BASE",
		"PERF_LARGE_RAW_BASE",
		"PERF_ELAPSED_TIME",
		"PERF_COUNTER_TEXT",
		"PERF_COUNTER_NODATA",
		"PERF_COUNTER_HISTOGRAM_TYPE"
	};

	TiXmlDocument* doc = Report::GetDocument("perf_type.xml");
	TiXmlElement* root = new TiXmlElement("types");
	TiXmlElement* node = NULL;

	for (int i=0;i<_countof(dwConstant);i++)
	{
		node = new TiXmlElement(szDescription[i]);
		char szBuffer[1024] = {0};
		sprintf(szBuffer, "0x%08X", dwConstant[i]);
		node->SetAttribute("value", szBuffer);
		root->LinkEndChild(node);
	}
	doc->LinkEndChild(root);
	doc->SaveFile();

	doc->Clear();
	delete doc;
	root = NULL;
	doc = NULL;
}

void AddToNode(std::vector<std::string> vecobj, std::vector<std::string> vecins, TiXmlElement* node)
{
	TiXmlElement* subNode = NULL;
	for (int i=0;i < vecobj.size();i++)
	{
		subNode = new TiXmlElement("object");
		subNode->SetAttribute("value", vecobj.at(i).c_str());
		node->LinkEndChild(subNode);
	}

	for (int i=0;i < vecins.size();i++)
	{
		subNode = new TiXmlElement("instance");
		subNode->SetAttribute("value", vecins.at(i).c_str());
		node->LinkEndChild(subNode);
	}
}

//TODO: add parameter support to display specified process
void GetMemoryCounters()
{
	PDH_RAW_COUNTER counter;
	DWORD dwType;
	LONGLONG llTimebase;

	getCounter(L"\\Process(explorer#1)\\Working Set", 0, counter, dwType);
	printf("explorer#1 Working Set : %s\n", Report::GetTextSize(counter.FirstValue).c_str());

	getCounter(L"\\Process(explorer#1)\\Working Set Peak", 0, counter, dwType);
	printf("explorer#1 Working Set Peak: %s\n", Report::GetTextSize(counter.FirstValue).c_str());

	getCounter(L"\\Process(explorer#1)\\Private Bytes", 0, counter, dwType);
	printf("explorer#1 Private Bytes : %s\n", Report::GetTextSize(counter.FirstValue).c_str());

	getCounter(L"\\Process(explorer#1)\\Private Bytes Peak", 0, counter, dwType);
	printf("explorer#1 Private Bytes Peak: %s\n", Report::GetTextSize(counter.FirstValue).c_str());

	getCounter(L"\\Process(explorer#1)\\Virtual Bytes", 0, counter, dwType);
	printf("explorer#1 Virtual Bytes : %s\n", Report::GetTextSize(counter.FirstValue).c_str());

	getCounter(L"\\Process(explorer#1)\\Virtual Bytes Peak", 0, counter, dwType);
	printf("explorer#1 Virtual Bytes Peak: %s\n", Report::GetTextSize(counter.FirstValue).c_str());

	getCounter(L"\\Process(explorer#1)\\Page File Bytes", 0, counter, dwType);
	printf("explorer#1 Page File Bytes : %s\n", Report::GetTextSize(counter.FirstValue).c_str());

	getCounter(L"\\Process(explorer#1)\\Pool Paged Bytes", 0, counter, dwType);
	printf("explorer#1 Pool Paged Bytes : %s\n", Report::GetTextSize(counter.FirstValue).c_str());

	getCounter(L"\\Process(explorer#1)\\Pool Nonpaged Bytes", 0, counter, dwType);
	printf("explorer#1 Pool Nonpaged Bytes : %s\n", Report::GetTextSize(counter.FirstValue).c_str());
}

#define MAX_MEMORY_COUNTER 64
#define MAX_TIME_COUNTER 64
void GetProcessMemoryCounters(LPCTSTR lpProcessName, DWORD dwIns, TiXmlElement* root)
{
	TiXmlElement* node = new TiXmlElement("process");
	TiXmlElement* subnode = NULL;

	PDH_RAW_COUNTER counter[MAX_MEMORY_COUNTER];
	DWORD dwType[MAX_MEMORY_COUNTER];
	LONGLONG llTimebase[MAX_MEMORY_COUNTER];

	PDH_RAW_COUNTER counter1[MAX_TIME_COUNTER];
	PDH_RAW_COUNTER counter2[MAX_TIME_COUNTER];
	DWORD dwTypeT[MAX_TIME_COUNTER];
	LONGLONG llTimebaseT[MAX_TIME_COUNTER];

	LONGLONG llDiv;
	LONGLONG llMul;
	LONGLONG llVal;

	WCHAR szBuffer[1024] = {0};
	if (dwIns != 0)
	{
		wsprintf(szBuffer, L"\\Process(%s#%d)\\", lpProcessName, dwIns);
	} 
	else
	{
		wsprintf(szBuffer, L"\\Process(%s)\\", lpProcessName);
	}
	
	std::wstring szCounterName;

	WCHAR* counterNames[] = {
		L"Working Set",
		L"Working Set Peak",
		L"Private Bytes",
		L"Private Bytes Peak",
		L"Virtual Bytes",
		L"Virtual Bytes Peak",
		L"Page File Bytes",
		L"Pool Paged Bytes",
		L"Pool Nonpaged Bytes",
		L"Thread Count",
		L"ID Process",
		L"Handle Count",
		L"Creating Process ID",
		L"Priority Base"
	};

	DWORD dwCounters = sizeof(counterNames) / sizeof(WCHAR*);

	for (int i=0;i<dwCounters;i++)
	{
		szCounterName = szBuffer;
		szCounterName += counterNames[i];
		getCounter(
			szCounterName.c_str(), 
			0, 
			counter[i], 
			dwType[i], 
			llTimebase[i]);
	}

	wprintf(L"\nProcess: %s\n", lpProcessName);
	node->SetAttribute("name", Report::ws2s(lpProcessName).c_str());
	if (dwIns != 0)
	{
		printf("Instance: %d\n", dwIns);
		node->SetAttribute("instance", dwIns);
	}

	for (int i=0;i<9;i++)
	{
		wprintf(L"%s: ", counterNames[i]);
		printf("%s\n", Report::GetTextSize(counter[i].FirstValue).c_str());
		
		subnode = new TiXmlElement("counter");
		subnode->SetAttribute("name", Report::ws2s(counterNames[i]).c_str());
		subnode->SetAttribute("value", Report::GetTextSize(counter[i].FirstValue).c_str());
		node->LinkEndChild(subnode);
	}

	for (int i=9;i<dwCounters;i++)
	{
		wprintf(L"%s: ", counterNames[i]);
		printf("%d\n", counter[i].FirstValue);

		subnode = new TiXmlElement("counter");
		subnode->SetAttribute("name", Report::ws2s(counterNames[i]).c_str());
		subnode->SetAttribute("value", counter[i].FirstValue);
		node->LinkEndChild(subnode);
	}


	WCHAR* TimeCountersNames[] = {
		L"Elapsed Time",
		L"% Processor Time",
		L"% User Time",
		L"% Privileged Time",
		L"IO Read Operations\/sec",
		L"IO Write Operations\/sec",
		L"IO Data Operations\/sec",
		L"IO Other Operations\/sec",
		L"IO Read Bytes\/sec",
		L"IO Write Bytes\/sec",
		L"IO Data Bytes\/sec",
		L"IO Other Bytes\/sec"		
	};

	DWORD dwTimeCounters = sizeof(TimeCountersNames) / sizeof(WCHAR*);

	for (int i=0;i<dwTimeCounters;i++)
	{
		szCounterName = szBuffer;
		szCounterName += TimeCountersNames[i];
		getCounter(szCounterName.c_str(), 
			0, 
			counter1[i], 
			dwTypeT[i], 
			llTimebaseT[i]);
	}

	Sleep(1000);

	for (int i=0;i<dwTimeCounters;i++)
	{
		szCounterName = szBuffer;
		szCounterName += TimeCountersNames[i];
		getCounter(szCounterName.c_str(), 
			0, 
			counter2[i]);
	}

	for (int i=0;i<dwTimeCounters;i++)
	{
// 		llDiv = counter2[i].SecondValue - counter1[i].SecondValue;
// 		llMul = counter2[i].FirstValue - counter1[i].FirstValue;
// 		llVal = llMul * 100 / llDiv;
		//wprintf(L"%s: %d\n", TimeCountersNames[i], llVal);
		wprintf(L"%s: ", TimeCountersNames[i]);
		printf("%s\n", getCounterValue(counter1[i], counter2[i], dwTypeT[i], llTimebaseT[i]).c_str());

		subnode = new TiXmlElement("counter");
		subnode->SetAttribute("name", Report::ws2s(TimeCountersNames[i]).c_str());
		subnode->SetAttribute("value", getCounterValue(counter1[i], counter2[i], dwTypeT[i], llTimebaseT[i]).c_str());
		node->LinkEndChild(subnode);
	}

	if (root != NULL)
	{
		root->LinkEndChild(node);
	}
}


void GetSystemCounters()
{
	PDH_RAW_COUNTER counter1[MAX_TIME_COUNTER];
	PDH_RAW_COUNTER counter2[MAX_TIME_COUNTER];
	PDH_RAW_COUNTER counter;
	DWORD dwType[MAX_TIME_COUNTER];
	LONGLONG llTimebase[MAX_TIME_COUNTER];
	PDH_FMT_COUNTERVALUE counterVal;
	LONGLONG llDiv;
	LONGLONG llMul;
	LONGLONG llVal;

	WCHAR* TimeCounters[] = {
		L"\\System\\File Read Operations\/sec",
		L"\\System\\File Write Operations\/sec",
		L"\\System\\File Control Operations\/sec",
		L"\\System\\File Read Bytes\/sec",
		L"\\System\\File Write Bytes\/sec",
		L"\\System\\File Control Bytes\/sec",
		L"\\System\\Context Switches\/sec"
	};

	char* TimeCountersNames[] = {
		"File Read Operations\/sec",
		"File Write Operations\/sec",
		"File Control Operations\/sec",
		"File Read Bytes\/sec",
		"File Write Bytes\/sec",
		"File Control Bytes\/sec",
		"Context Switches\/sec"
	};

	DWORD dwTimeCounters = sizeof(TimeCounters) / sizeof(WCHAR*);

	for (int i=0;i<dwTimeCounters;i++)
	{
		getCounter(TimeCounters[i], 
			0, 
			counter1[i], 
			dwType[i], 
			llTimebase[i]);
	}

	Sleep(1000);

	for (int i=0;i<dwTimeCounters;i++)
	{
		getCounter(TimeCounters[i], 
			0, 
			counter2[i]);
	}

	for (int i=0;i<dwTimeCounters;i++)
	{
		llDiv = counter2[i].SecondValue - counter1[i].SecondValue;
		llMul = counter2[i].FirstValue - counter1[i].FirstValue;
		llVal = llMul * llTimebase[i] / llDiv;
		printf("%s: %d\n", TimeCountersNames[i], llVal);
	}

	int i = dwTimeCounters;
	getCounter(L"\\System\\Processes", 0, counter);
	printf("Processes: %d\n", counter.FirstValue);

	i++;
	getCounter(L"\\System\\Threads", 0, counter);
	printf("Threads: %d\n", counter.FirstValue);

	i++;
	getCounter(L"\\System\\Processor Queue Length", 0, counter);
	printf("Processor Queue Length: %d\n", counter.FirstValue);

	i++;
	getCounter(L"\\System\\System Up Time", 10, counter1[i], dwType[i], llTimebase[i]);
	printf("System Up Time: %s\n", 
		getCounterValue(counter1[i], counter1[i], dwType[i], llTimebase[i]).c_str());
}

void GetProcessCounters(std::vector<std::string> vecIns)
{
	TiXmlDocument* doc = Report::GetDocument("processCounters.xml");
	TiXmlElement* root = new TiXmlElement("processes");

	std::map<std::string, DWORD> mapIns;
	for (int i=0;i<vecIns.size();i++)
	{
		if (mapIns.find(vecIns.at(i)) == mapIns.end())
		{
			mapIns[vecIns.at(i)] = 0;
		}
		mapIns[vecIns.at(i)] ++;
	}

	WCHAR szBuffer[1024] = {0};
	for (std::map<std::string, DWORD>::iterator it = mapIns.begin();
		it != mapIns.end();
		it ++)
	{
		memset(szBuffer, 0, 1024 * sizeof(WCHAR));
		MultiByteToWideChar(CP_ACP,
			NULL,
			it->first.c_str(),
			it->first.length(),
			szBuffer,
			1024);

		if (it->second > 1)
		{
			for (int i=0;i<it->second;i++)
			{
				GetProcessMemoryCounters(szBuffer, i+1, root);
				break;
			}
		}
		else
		{
			GetProcessMemoryCounters(szBuffer, 0, root);
			break;
		}
	}

	doc->LinkEndChild(root);
	doc->SaveFile();

	doc->Clear();
	delete doc;
	root = NULL;
	doc = NULL;
}

pdhProcessEnumerator::pdhProcessEnumerator()
	:m_bWork(TRUE)
{

}

pdhProcessEnumerator::~pdhProcessEnumerator()
{


}

void pdhProcessEnumerator::Initialize()
{
	m_doc = Report::GetDocument("objins.xml");
	m_root = new TiXmlElement("objins");
}

void pdhProcessEnumerator::Enumerate()
{
	GetSystemCounters();

	std::vector<std::string> vecObj;
	std::vector<std::string> vecIns;

	TiXmlElement* node = NULL;
	
	enumObjIns(L"Process", vecObj, vecIns);
	node = new TiXmlElement("Process");
	AddToNode(vecObj, vecIns, node);
	m_root->LinkEndChild(node);

	TiXmlElement* node1 = new TiXmlElement("enumProcess");
	GetProcessMemoryCounters(L"enumProcess", 0, node1);
	m_root->LinkEndChild(node1);

	GetProcessCounters(vecIns);

	GetProcessMemoryCounters(L"enumProcess", 0, NULL);

	vecObj.clear();
	vecIns.clear();
	enumObjIns(L"Processor", vecObj, vecIns);
	node = new TiXmlElement("Processor");
	AddToNode(vecObj, vecIns, node);
	m_root->LinkEndChild(node);

	vecObj.clear();
	vecIns.clear();
	enumObjIns(L"System", vecObj, vecIns);
	node = new TiXmlElement("System");
	AddToNode(vecObj, vecIns, node);
	m_root->LinkEndChild(node);

	vecObj.clear();
	vecIns.clear();
	enumObjIns(L"Memory", vecObj, vecIns);
	node = new TiXmlElement("Memory");
	AddToNode(vecObj, vecIns, node);
	m_root->LinkEndChild(node);

	vecObj.clear();
	vecIns.clear();
	enumObjIns(L"Thread", vecObj, vecIns);
	node = new TiXmlElement("Thread");
	AddToNode(vecObj, vecIns, node);
	m_root->LinkEndChild(node);

	vecObj.clear();
	vecIns.clear();
	enumObjIns(L"PhysicalDisk", vecObj, vecIns);
	node = new TiXmlElement("PhysicalDisk");
	AddToNode(vecObj, vecIns, node);
	m_root->LinkEndChild(node);

	vecObj.clear();
	vecIns.clear();
	enumObjIns(L"LogicalDisk", vecObj, vecIns);
	node = new TiXmlElement("LogicalDisk");
	AddToNode(vecObj, vecIns, node);
	m_root->LinkEndChild(node);

	

//	SavePerfTypes();

// 	GetProcessMemoryCounters(L"devenv", 0);
// 	GetProcessMemoryCounters(L"enumProcess", 0);
}



void pdhProcessEnumerator::Finalize()
{
	m_doc->LinkEndChild(m_root);
	m_doc->SaveFile();

	m_doc->Clear();
	delete m_doc;
	m_root = NULL;
	m_doc = NULL;
}

