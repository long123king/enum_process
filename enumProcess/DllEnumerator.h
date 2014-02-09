#ifndef __ENUM_PROCESS_DLL_ENUMERATOR_H__
#define __ENUM_PROCESS_DLL_ENUMERATOR_H__

#include "IPidReceiver.h"
#include "tinyxml.h"
#include "report.h"
#include <winternl.h>   // for Windows internal declarations.


class DllEnumerator : public IPidReceiver
{
public:
	static DllEnumerator* GetInstance()
	{
		static DllEnumerator _instance;
		return &_instance;
	}

	virtual void InitializeReceiver();
	virtual void ReceivePid(DWORD dwPid);
	virtual void FinalizeReceiver();

private:
	DllEnumerator();
	~DllEnumerator();

	void printPEB(PPEB ppeb, TiXmlElement* process);

	void printLdr(PPEB_LDR_DATA ldr, HANDLE hProcess, TiXmlElement* process);

	void printParameters(PRTL_USER_PROCESS_PARAMETERS parameters,
		HANDLE hProcess,
		TiXmlElement* process);

private:
	TiXmlDocument* m_doc;
	TiXmlElement* m_root;
	bool m_bWork;
};

#endif//__ENUM_PROCESS_DLL_ENUMERATOR_H__