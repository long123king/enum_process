#include "PerformanceInfo.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

void PerformanceInfo::Initialize()
{
	m_doc = Report::GetDocument("performance.xml");
	if (m_doc == NULL)
	{
		m_bWork = false;
	}

	if (m_bWork)
	{
		m_root = new TiXmlElement("Performance");
	}	
}

void PerformanceInfo::Enumerate()
{
	PERFORMACE_INFORMATION pi;
	DWORD dwLen = sizeof(pi);
	BOOL bRet = GetPerformanceInfo(
		&pi,
		dwLen);

	if (!bRet)
	{
		return;
	}

	TiXmlElement* node = new TiXmlElement("CommitTotal");
	node->SetAttribute("value", pi.CommitTotal);
	m_root->LinkEndChild(node);


	node = new TiXmlElement("CommitLimit");
	node->SetAttribute("value", pi.CommitLimit);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("CommitPeak");
	node->SetAttribute("value", pi.CommitPeak);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("PhysicalTotal");
	node->SetAttribute("value", pi.PhysicalTotal);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("PhysicalAvailable");
	node->SetAttribute("value", pi.PhysicalAvailable);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("SystemCache");
	node->SetAttribute("value", pi.SystemCache);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("KernelTotal");
	node->SetAttribute("value", pi.KernelTotal);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("KernelPaged");
	node->SetAttribute("value", pi.KernelPaged);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("KernelNonpaged");
	node->SetAttribute("value", pi.KernelNonpaged);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("PageSize");
	node->SetAttribute("value", pi.PageSize);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("HandleCount");
	node->SetAttribute("value", pi.HandleCount);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("ProcessCount");
	node->SetAttribute("value", pi.ProcessCount);
	m_root->LinkEndChild(node);

	node = new TiXmlElement("ThreadCount");
	node->SetAttribute("value", pi.ThreadCount);
	m_root->LinkEndChild(node);
	
}

void PerformanceInfo::Finalize()
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

PerformanceInfo::PerformanceInfo()
	:m_bWork(true)
{

}

PerformanceInfo::~PerformanceInfo()
{

}
