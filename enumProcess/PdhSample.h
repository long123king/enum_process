#ifndef __ENUM_PROCESS_PDH_SAMPLES_H__
#define __ENUM_PROCESS_PDH_SAMPLES_H__

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>        
#include <io.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <strsafe.h>
#include <winperf.h>
#include <String>
#include <vector>
#include <map>
#include <iostream>
using namespace std;

#define INITIAL_SIZE        51200
#define EXTEND_SIZE         25600
#define PERF_SUBKEY_GLOBAL	_T("Global")
#define REGSUBKEY_COUNTERS  _T("Counters")

typedef map <DWORD, DWORD>::const_iterator CIT;

class PdhSample
{
public:
	static BOOL ConnectComputerPerformanceRegistry(LPCWSTR lpMachineName, HKEY &hKey);
	static BOOL GetNameStrings(HKEY hKey, map <DWORD, LPWSTR> &mPerfObjectIndex);
	static BOOL EnumPerfObjects(HKEY hKey, vector <PERF_OBJECT_TYPE> &vPerfObjects);
	static BOOL LoadObjectData(HKEY hKey, DWORD dwObjIndex, map <DWORD, LPWSTR> mPerfCountersIndex);

private:
	static PPERF_OBJECT_TYPE FirstObject(PPERF_DATA_BLOCK PerfData)   
	{   
		return((PPERF_OBJECT_TYPE)((PBYTE)PerfData + PerfData->HeaderLength));   
	}   

	static PPERF_OBJECT_TYPE NextObject(PPERF_OBJECT_TYPE PerfObj)   
	{   
		return((PPERF_OBJECT_TYPE)((PBYTE)PerfObj + PerfObj->TotalByteLength));   
	}
};



#endif//__ENUM_PROCESS_PDH_SAMPLES_H__