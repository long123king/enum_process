#include "PdhSample.h"
#include "report.h"
#include "tinyxml.h"
void printObjNames(map <DWORD, LPWSTR> &mPerfObjectIndex)
{
	TiXmlDocument* doc = Report::GetDocument("counterName.xml");
	TiXmlElement* root = new TiXmlElement("counters");

	TiXmlElement* node = NULL;

	
	for (map<DWORD, LPWSTR>::iterator it = mPerfObjectIndex.begin();
		it != mPerfObjectIndex.end();
		it++)
	{
		node = new TiXmlElement("counter");
		node->SetAttribute("index", it->first);
		char szBuffer[1024] = {0};
		WideCharToMultiByte(
			CP_ACP,
			NULL,
			it->second,
			wcslen(it->second),
			szBuffer,
			1024,
			NULL,
			NULL);
		node->SetAttribute("name", szBuffer);
		root->LinkEndChild(node);
	}

	doc->LinkEndChild(root);
	doc->SaveFile();	

	doc->Clear();
	delete doc;
	root = NULL;
	doc = NULL;
}

BOOL PdhSample::ConnectComputerPerformanceRegistry(LPCWSTR lpMachineName, HKEY &hKey)
{
	DWORD retCode;

	retCode = RegConnectRegistry(lpMachineName, HKEY_PERFORMANCE_DATA, &hKey);
	return (retCode == ERROR_SUCCESS) ? TRUE : FALSE;
}

BOOL PdhSample::EnumPerfObjects(HKEY hKey, vector <PERF_OBJECT_TYPE> &vPerfObjects)
{
	DWORD  retCode = ERROR_MORE_DATA;
	DWORD  dwType = 0;
	DWORD  dwSize = 0;
	PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK)malloc(INITIAL_SIZE);
	PPERF_OBJECT_TYPE PPerfObj;

	while (retCode == ERROR_MORE_DATA)
	{
		dwSize += EXTEND_SIZE;
		PerfData = (PPERF_DATA_BLOCK)realloc(PerfData, dwSize); 
		retCode = RegQueryValueEx(hKey, PERF_SUBKEY_GLOBAL, 0, &dwType, (LPBYTE)PerfData, &dwSize);
	}
	if (retCode != ERROR_SUCCESS)
		return FALSE;

	PPerfObj = FirstObject(PerfData);
	for (int i = 0; i < PerfData->NumObjectTypes; i++)
	{
		PERF_OBJECT_TYPE PerfObj;
		memcpy(&PerfObj, PPerfObj, sizeof(PERF_OBJECT_TYPE));
		vPerfObjects.push_back(PerfObj);
		PPerfObj = NextObject(PPerfObj);
	}

	return TRUE;
}

BOOL PdhSample::GetNameStrings(HKEY hKey, map <DWORD, LPWSTR> &mPerfObjectIndex)
{
	DWORD  retCode = 0;
	DWORD  dwType = 0;
	DWORD  dwSize = 0;
	LPBYTE buffer = NULL;
	LPWSTR  p, p2; 
	LPWSTR  lpCurrentString;
	DWORD  dwCounter = 0;

	retCode = RegQueryValueEx(hKey, REGSUBKEY_COUNTERS, NULL, &dwType, NULL, &dwSize);
	if (retCode != ERROR_SUCCESS)
	{
		perror("RegQueryValueEx REGSUBKEY_COUNTERS Size");
		return FALSE;
	}

	buffer = (LPBYTE)malloc(dwSize);
	memset(buffer, 0, dwSize);
	retCode = RegQueryValueEx(hKey, REGSUBKEY_COUNTERS, NULL, &dwType, buffer, &dwSize);
	if (retCode != ERROR_SUCCESS)
	{
		perror("RegQueryValueEx REGSUBKEY_COUNTERS Content");
		return FALSE;
	}

	p = (LPWSTR) buffer;
	for(lpCurrentString = p; *lpCurrentString; lpCurrentString  += (wcslen(lpCurrentString) + 1))   
	{   
		dwCounter = _wtol(lpCurrentString);   
		lpCurrentString += (wcslen(lpCurrentString) + 1);      
		mPerfObjectIndex[dwCounter] = (LPWSTR)lpCurrentString;
	}

	printObjNames(mPerfObjectIndex);

	return TRUE;
}

BOOL PdhSample::LoadObjectData(HKEY hKey, DWORD dwObjIndex, map <DWORD, LPWSTR> mPerfCountersIndex)
{
	TCHAR  szSubKey[1024] = {0};
	DWORD  rt = 0;
	DWORD  dwType = 0;
	DWORD  dwSize = 0;
	LPBYTE buffer = NULL;
	BOOL   bPass = TRUE;
	map <DWORD, string> mCounters;
	map <DWORD, DWORD> mOffsets;
	map <DWORD, int> mIndexs;

	LPWSTR p;
	PPERF_DATA_BLOCK             pPerf;
	PPERF_OBJECT_TYPE            pObj;
	PPERF_INSTANCE_DEFINITION    pInst;
	PPERF_COUNTER_BLOCK          pCounter;
	PPERF_COUNTER_DEFINITION     pCounterDef;

	buffer = NULL;
	dwSize = INITIAL_SIZE;
	buffer = (LPBYTE) malloc(dwSize);
	memset(buffer, 0, dwSize);
	wsprintf(szSubKey, L"%u", dwObjIndex);
	// get Object PERF_DATA_BLOCK
	while (bPass)
	{
		rt = RegQueryValueEx(hKey, szSubKey, NULL, &dwType, buffer, &dwSize);
		pPerf = (PPERF_DATA_BLOCK) buffer;
		if ((rt == ERROR_SUCCESS) && (dwSize > 0) &&
			pPerf->Signature[0] == (WCHAR)'P' &&
			pPerf->Signature[1] == (WCHAR)'E' &&
			pPerf->Signature[2] == (WCHAR)'R' &&
			pPerf->Signature[3] == (WCHAR)'F')
		{
			break;
		}

		if (rt == ERROR_MORE_DATA)
		{
			dwSize += EXTEND_SIZE;
			buffer  = (LPBYTE)realloc(buffer, dwSize );
			memset(buffer, 0, dwSize );
		}
		else
		{
			bPass = FALSE;
		}
	}

	if (bPass)
	{
		// get Counters
		pObj = (PPERF_OBJECT_TYPE) ((DWORD)pPerf + pPerf->HeaderLength);
		pCounterDef = (PPERF_COUNTER_DEFINITION) ((DWORD)pObj + pObj->HeaderLength); 
//		pCListCtrlInstances->InsertColumn(0, (LPCTSTR)(mPerfCountersIndex[dwObjIndex]), LVCFMT_LEFT, 120);
		for (DWORD i = 0; i < (DWORD)pObj->NumCounters; i++) 
		{ 
			mOffsets[pCounterDef->CounterNameTitleIndex] = pCounterDef->CounterOffset;
			mIndexs[pCounterDef->CounterNameTitleIndex] = i + 1;
//			pCListBoxCounters->InsertString(i, (LPCTSTR)(mPerfCountersIndex[pCounterDef->CounterNameTitleIndex]));
//			pCListCtrlInstances->InsertColumn(i + 1, (LPCTSTR)(mPerfCountersIndex[pCounterDef->CounterNameTitleIndex]), LVCFMT_LEFT, 120);
			pCounterDef++; 
		} 

		// get instances
		if (pObj->NumInstances == 0 || PERF_NO_INSTANCES == pObj->NumInstances)
		{
			// no instances
			pCounter = (PPERF_COUNTER_BLOCK)((DWORD)pObj + pObj->DefinitionLength);
//			pCListCtrlInstances->InsertItem(0, "None");
			for(CIT p = mOffsets.begin(); p != mOffsets.end(); ++p)
			{
				DWORD counterVal = *((LPDWORD) ((DWORD)pCounter + p->second));
				char info[256] = {0};
				sprintf(info, "%u", counterVal);
				printf("%s\n", info);
//				pCListCtrlInstances->SetItemText(0, mIndexs[p->first], info);
			}
		}
		else 
		{
			// multi-instances case
			char  szInstanceName[MAX_PATH];
			pInst = (PPERF_INSTANCE_DEFINITION) ((DWORD)pObj + pObj->DefinitionLength);
			for (DWORD i = 0; i < (DWORD)pObj->NumInstances; i++)
			{
				p  = (LPWSTR) ((DWORD)pInst + pInst->NameOffset);
				rt = WideCharToMultiByte(
					CP_ACP, 
					0, 
					(LPCWSTR)p, 
					-1, 
					szInstanceName, 
					MAX_PATH, 
					NULL, 
					NULL);
//				pCListCtrlInstances->InsertItem(i, szInstanceName);
				printf("Instance Name: %s\n", szInstanceName);

				for(CIT p = mOffsets.begin(); p != mOffsets.end(); ++p)
				{
					pCounter = (PPERF_COUNTER_BLOCK) ((DWORD)pInst + pInst->ByteLength);
					DWORD counterVal = *((LPDWORD) ((DWORD)pCounter + p->second));

					char info[256] = {0};
					sprintf(info, "%u", counterVal);
					printf("%s\n", info);
//					pCListCtrlInstances->SetItemText(i, mIndexs[p->first], info);
				}
				// Point to the next process 
				pInst = (PPERF_INSTANCE_DEFINITION) ((DWORD)pCounter + pCounter->ByteLength);
			}
		}
	}// end if (bPass)

	if (buffer) 
	{
		free(buffer);
		buffer = NULL;
	}
	return bPass;
}