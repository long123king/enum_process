// enumProcess.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "IPidReceiver.h"
#include "DllEnumerator.h"
#include "MemoryRegionEnumerator.h"
#include "ProcessEnumerator.h"
#include "DriverEnumerator.h"
#include "PerformanceInfo.h"
#include "ProcessMemoryInfo.h"
#include "thProcessEnumerator.h"
#include "thHeapEnumerator.h"
#include "thModuleEnumerator.h"
#include "thThreadEnumerator.h"
#include "Log.h"
#include "PdhTest.h"
#include "PerfObjEnumerator.h"
#include "PdhProcess.h"
#include "pdhProcessEnumerator.h"
#include "SqliteWritter.h"

#include <map>
#include <vector>


int _tmain(int argc, _TCHAR* argv[])
{
	LOG_ON;
//	ProcessEnumerator::GetInstance()->AddReceiver(DllEnumerator::GetInstance());
// 	ProcessEnumerator::GetInstance()->AddReceiver(MemoryRegionEnumerator::GetInstance());
// 	ProcessEnumerator::GetInstance()->AddReceiver(ProcessMemoryInfo::GetInstance());
// 	ProcessEnumerator::GetInstance()->AddReceiver(thHeapEnumerator::GetInstance());
// 	ProcessEnumerator::GetInstance()->AddReceiver(thModuleEnumerator::GetInstance());
// 	ProcessEnumerator::GetInstance()->AddReceiver(thThreadEnumerator::GetInstance());

//	ProcessEnumerator::GetInstance()->Enumerate();

// 	DriverEnumerator::GetInstance()->Initialize();
// 	DriverEnumerator::GetInstance()->Enumerate();
// 	DriverEnumerator::GetInstance()->Finalize();
// 
// 	PerformanceInfo::GetInstance()->Initialize();
// 	PerformanceInfo::GetInstance()->Enumerate();
// 	PerformanceInfo::GetInstance()->Finalize();

// 	thProcessEnumerator::GetInstance()->AddReceiver(thHeapEnumerator::GetInstance());
// 	thProcessEnumerator::GetInstance()->AddReceiver(thModuleEnumerator::GetInstance());
// 	thProcessEnumerator::GetInstance()->AddReceiver(thThreadEnumerator::GetInstance());
//	thProcessEnumerator::GetInstance()->Enumerate();

//	PdhTest::GetInstance()->test();


// 	PerfObjEnumerator::GetInstance()->Initialize();
// 	PerfObjEnumerator::GetInstance()->Enumerate();
// 	PerfObjEnumerator::GetInstance()->Finalize();

//	test_pdh_process();

	pdhProcessEnumerator::GetInstance()->Initialize();
	pdhProcessEnumerator::GetInstance()->Enumerate();
	pdhProcessEnumerator::GetInstance()->Finalize();

//	SqliteWritter::test();

	return 0;
}

