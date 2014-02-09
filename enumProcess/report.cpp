#include "report.h"

NameDocMap Report::m_map;

Report::Report()
{

}

Report::~Report()
{

}

TiXmlDocument* Report::GetDocument(const char* filename)
{
	NameDocMap::iterator it = m_map.find(filename);
	if (it == m_map.end())
	{
		TiXmlDocument* doc = new TiXmlDocument(filename);
		if (doc != NULL)
		{
			m_map[filename] = doc;
			return doc;
		}
			

	}
	
	return NULL;
}

