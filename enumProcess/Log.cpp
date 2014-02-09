#include "Log.h"

void Log::v( const char* cmd, ... )
{
	if (!m_bWork)
		return;

	va_list args;

	va_start(args,cmd);
	vprintf(cmd,args);
	va_end(args);
}

Log::Log()
{

}

Log::~Log()
{

}

