#ifndef __ENUM_PROCESS_REPORT_H__
#define __ENUM_PROCESS_REPORT_H__

#include "tinyxml.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <map>
typedef std::map<std::string, TiXmlDocument*> NameDocMap;

class Report
{
	
public:
	static TiXmlDocument* GetDocument(const char* filename);

private:
	Report();
	~Report();
public:
	static std::string GetTextSize(DWORD dwBytes)
	{
		char szText[1024] = {0};
		if (dwBytes < 1024)
		{
			sprintf(szText, "%d B", dwBytes);
		}
		else if (dwBytes < 1024 * 1024)
		{
			double dbBytes = dwBytes;
			sprintf(szText, "%0.2f KB", dbBytes / 1024.);
		}
		else if (dwBytes < 1024 * 1024 * 1024)
		{
			double dbBytes = dwBytes;
			sprintf(szText, "%0.2f MB", dbBytes / 1024. / 1024.);
		}
		else/* if (dwBytes < 1024 * 1024 * 1024 * 1024)*/
		{
			double dbBytes = dwBytes;
			sprintf(szText, "%0.2f GB", dbBytes / 1024. / 1024. / 1024.);
		}
		return szText;
	}
	static std::string GetTextTime(DWORD dwMillis)
	{
		//dwMillis /= 1000000;
		DWORD dwMilliSeconds = 0;
		DWORD dwSeconds = 0;
		DWORD dwMinutes = 0;
		DWORD dwHours = 0;

		DWORD dwTmp = dwMillis;
		char szBuffer[1024] = {0};
		std::string strTime = " ";
		dwMilliSeconds = dwMillis % 1000;
		dwTmp = dwMillis / 1000;
		dwSeconds = dwTmp % 60;
		dwTmp /= 60;
		dwMinutes = dwTmp % 60;
		dwTmp /= 60;
		dwHours = dwTmp;

		if (dwHours != 0)
		{
			sprintf(szBuffer, "%d hour, %2d min, %2d sec", dwHours, dwMinutes, dwSeconds);
		}
		else if (dwMinutes != 0)
		{
			sprintf(szBuffer, "%2d min, %2d sec", dwMinutes, dwSeconds);
		}
		else if (dwSeconds != 0)
		{
			sprintf(szBuffer, "%2d sec", dwSeconds);
		}
		else
		{
			sprintf(szBuffer, "%d millis", dwMilliSeconds);
		}
		
		strTime = szBuffer;
		return strTime;
	}
	static std::string ws2s(const std::wstring& ws)
	{
		const wchar_t* _Source = ws.c_str();
		size_t _Dsize = 2 * ws.size() + 1;
		char *_Dest = new char[_Dsize];
		memset(_Dest,0,_Dsize);
		wcstombs(_Dest,_Source,_Dsize);
		std::string result = _Dest;
		delete []_Dest;
		return result;
	}
	static std::wstring s2ws(const std::string& s)
	{
		const char* _Source = s.c_str();
		size_t _Dsize = s.size() + 1;
		wchar_t *_Dest = new wchar_t[_Dsize];
		wmemset(_Dest, 0, _Dsize);
		mbstowcs(_Dest,_Source,_Dsize);
		std::wstring result = _Dest;
		delete []_Dest;
		return result;
	}

private:
	static NameDocMap m_map; // filename map TiXmlElement(m_root)
};

#endif//__ENUM_PROCESS_REPORT_H__