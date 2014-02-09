#include<windows.h>
#include<tchar.h>
#include<stdio.h>
#include "PdhProcess.h"

#define INITIAL_SIZE        51200
#define EXTEND_SIZE         25600
#define REGKEY_PERF        _T("Software\\Microsoft\\Windows NT\\Currentversion\\Perflib")
#define REGSUBKEY_COUNTERS _T("Counters")
#define PROCESS_COUNTER    _T("process")
#define PROCESSID_COUNTER   _T("id process") 

#include <vector>
#include "report.h"
#include "PdhSample.h"
 
void test_pdh_process()
{
    LANGID lid = MAKELANGID(LANG_ENGLISH,SUBLANG_NEUTRAL);
    TCHAR szSubKey[1024];
    _stprintf(szSubKey, _T("%s\\%03x"),REGKEY_PERF, lid);
    HKEY hSubKey;
    DWORD rt = RegOpenKeyEx(HKEY_LOCAL_MACHINE,szSubKey, 0, 
        KEY_READ, &hSubKey);
    if (rt != ERROR_SUCCESS)
    {
        return ;
    }
    DWORD dwType = 0;
    DWORD dwSize = 0;
    LPBYTE buffer = NULL;
    BOOL  pass = FALSE;
    // Get the buffer size for the counter names
    rt = RegQueryValueEx(hSubKey,REGSUBKEY_COUNTERS, NULL,
        &dwType, NULL, &dwSize);
    if (rt == ERROR_SUCCESS)
    {
        buffer = (LPBYTE) malloc(dwSize);
        memset(buffer, 0, dwSize);
        // Read the counter names from theregistry
        rt = RegQueryValueEx(hSubKey,REGSUBKEY_COUNTERS, NULL,
            &dwType, buffer, &dwSize);
    }
    LPWSTR p, p2;
    DWORD dwProcessIdTitle; 
    DWORD dwProcessIdCounter; 
    PPERF_DATA_BLOCK             pPerf;
    PPERF_OBJECT_TYPE            pObj;
    PPERF_INSTANCE_DEFINITION    pInst;
    PPERF_COUNTER_BLOCK          pCounter;
    PPERF_COUNTER_DEFINITION     pCounterDef;
	std::map<DWORD, LPWSTR> strMap;
    if (rt == ERROR_SUCCESS)
    {
        pass = TRUE;
        // Now loop thru the counter nameslooking for the "Process" counters:
        // the buffer contains multiple nullterminated strings and then
        // finally null terminated at theend.  the strings are in pairs of
        // counter number and counter name.
//         p = (LPWSTR) buffer;
//         while (*p) 
//         {
//             if (p > (LPWSTR) buffer) 
//             {
//                 for (p2 = p - 2; _istdigit(*p2);p2--)
//                     ;
//             }
//             if (_tcsicmp(p, PROCESS_COUNTER) ==0)
//             {
//                 // Look backwards for thecounter number
//                 for (p2 = p - 2; _istdigit(*p2);p2--) 
//                     ;
//                 _tcscpy(szSubKey, p2+1);
//             } 
//             else if (wcsicmp(p,PROCESSID_COUNTER) == 0) 
//             {
//                 // Look backwards for thecounter number
//                 for (p2 = p - 2; _istdigit(*p2);p2--) 
//                     ; 
//                 dwProcessIdTitle = _wtol(p2 + 1);
//             }
//             // Point to the next string
//             p += (_tcslen(p) + 1);
//         }
//  
//         // Free the counter names buffer
//         free(buffer);
//         buffer = NULL;
        // Allocate the initial buffer for theperformance data
		
		PdhSample::GetNameStrings(HKEY_PERFORMANCE_DATA, strMap);
		for (map<DWORD, LPWSTR>::iterator it = strMap.begin();
			it != strMap.end();
			it++)
		{
			if (wcsicmp(it->second, PROCESS_COUNTER) == 0)
			{
				_stprintf(szSubKey, L"%d", it->first);
				break;
			}
		}
        dwSize = INITIAL_SIZE;
        buffer = (LPBYTE) malloc(dwSize);
        memset(buffer, 0, dwSize);
        while (pass)
        {
            rt =RegQueryValueEx(HKEY_PERFORMANCE_DATA, szSubKey, NULL,
                &dwType, buffer,&dwSize);
            pPerf = (PPERF_DATA_BLOCK) buffer;
            // Check for success and valid perfdata block signature
            if ((rt == ERROR_SUCCESS) &&(dwSize > 0) &&
                pPerf->Signature[0] ==(WCHAR)'P' &&
                pPerf->Signature[1] ==(WCHAR)'E' &&
                pPerf->Signature[2] ==(WCHAR)'R' &&
                pPerf->Signature[3] ==(WCHAR)'F')
            {
                break;
            }
            // If buffer is not big enough,reallocate and try again
            if (rt == ERROR_MORE_DATA)
            {
                dwSize += EXTEND_SIZE;
                buffer  = (LPBYTE) realloc(buffer, dwSize );
                memset(buffer, 0, dwSize );
            }
            else
            {
                pass = FALSE;
            }
        }
    }
    if (pass)
    {
        DWORD i;
        // Set the perf_object_type pointer
        pObj = (PPERF_OBJECT_TYPE) ((DWORD)pPerf+ pPerf->HeaderLength);
        // loop thru the performance counterdefinition records looking 
        // for the process id counter and thensave its offset 
        pCounterDef = (PPERF_COUNTER_DEFINITION)((DWORD)pObj + pObj->HeaderLength); 
		std::vector<DWORD> vecOffset;
		std::vector<DWORD> vecNameIndex;
        for (i = 0; i <(DWORD)pObj->NumCounters; i++) 
        { 
//             if(pCounterDef->CounterNameTitleIndex == dwProcessIdTitle) 
//             { 
//                 dwProcessIdCounter =pCounterDef->CounterOffset; 
//                 break; 
//             } 
			vecOffset.push_back(pCounterDef->CounterOffset);
			vecNameIndex.push_back(pCounterDef->CounterNameTitleIndex);
            pCounterDef++; 
        } 

		TiXmlDocument* doc = Report::GetDocument("Counters_process.xml");
		TiXmlElement* root = new TiXmlElement("processes");

		TiXmlElement* node = NULL;
		TiXmlElement* subNode = NULL;
        
        // Loop thru the performance instancedata extracting each process name
        char szProcessName[MAX_PATH];
        pInst = (PPERF_INSTANCE_DEFINITION)((DWORD)pObj + pObj->DefinitionLength);
        for (i = 0; i <(DWORD)pObj->NumInstances; i++)
        {
            // Point to the process name
            p = (LPWSTR) ((DWORD)pInst + pInst->NameOffset);
            rt = WideCharToMultiByte(
				CP_ACP, 
				0,
				(LPCWSTR)p, 
				-1, 
				szProcessName,
                sizeof(szProcessName), 
				NULL,
				NULL);

			node = new TiXmlElement("process");
			node->SetAttribute("name", szProcessName);

            // Get the process id
            pCounter = (PPERF_COUNTER_BLOCK)((DWORD)pInst + pInst->ByteLength);
           // DWORD processId = *((LPDWORD)((DWORD)pCounter + dwProcessIdCounter));
			for (int j=0;j<vecOffset.size();j++)
			{
				DWORD dwNameIndex = vecNameIndex.at(j);
				DWORD dwValue = *((LPDWORD)((DWORD)pCounter + vecOffset.at(j)));
//				LPBYTE lpBuffer = (LPBYTE)pCounter + vecOffset.at(j);

				
				LPWSTR lpwName = strMap.find(dwNameIndex)->second;
				char szBuffer[1024] = {0};

				WideCharToMultiByte(
					CP_ACP,
					NULL,
					lpwName,
					wcslen(lpwName),
					szBuffer,
					1024,
					NULL,
					NULL);

				subNode = new TiXmlElement("counter");				
				subNode->SetAttribute("name", szBuffer);
				//->SetAttribute("value", dwValue);
				if (stricmp(szBuffer, "% Processor Time") == 0 ||
					stricmp(szBuffer, "% User Time") == 0 ||
					stricmp(szBuffer, "% Privileged Time") == 0 ||
					stricmp(szBuffer, "Elapsed Time") == 0)
				{
					subNode->SetAttribute("value", Report::GetTextTime(dwValue).c_str());
				}
				else
					subNode->SetAttribute("value", Report::GetTextSize(dwValue).c_str());
				node->LinkEndChild(subNode);
			}
//             if (strcmp(szProcessName,"System") && processId)
//             {
//                 printf("%s Pid:%d\n",szProcessName, processId);
//             }
            // Point to the next process 
            pInst = (PPERF_INSTANCE_DEFINITION)((DWORD)pCounter + pCounter->ByteLength);

			root->LinkEndChild(node);
        }

		doc->LinkEndChild(root);
		doc->SaveFile();

		doc->Clear();
		delete doc;
		root = NULL;
		doc = NULL;

    }
    if (buffer) 
    {
        free(buffer);
        buffer = NULL;
    }
    RegCloseKey(hSubKey);
    RegCloseKey(HKEY_PERFORMANCE_DATA);
  //  getchar();
    return ;
}