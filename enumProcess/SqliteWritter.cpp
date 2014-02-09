#include "SqliteWritter.h"
#include <stdio.h>
#include "sqlite3.h"
#include <string>
#include <vector>
#include <map>

static int _callback_exec(void * notused,int argc, char ** argv, char ** aszColName)  
{  
	int i;  
	for ( i=0; i<argc; i++ )  
	{  
		printf( "%s = %s\n", aszColName[i], argv[i] == 0 ? "NUL" : argv[i] );  
	}  
	return 0;  
}  

SqliteWritter::SqliteWritter()
{

}

SqliteWritter::~SqliteWritter()
{

}

std::string GetSelectAllString(const char* table)
{
	char szBuffer[1024] = {0};
	sprintf(szBuffer, "SELECT * from %s;", table);
	return szBuffer;
}

std::string GetCreateTableString(const char* table, 
	std::map<std::string,std::string> columes)
{
	std::string str = "CREATE TABLE ";
	str += table;
	str += "(";

	for (std::map<std::string, std::string>::iterator it = columes.begin();
		it != columes.end();
		it++)
	{
		if (it != columes.begin())
			str += ",";

		str += "\'";
		str += it->first.c_str();
		str += "\'";
		str += " ";
		str += it->second.c_str();
	}
	str += ");";
	return str;
}

std::string GetInsertString(const char* table, 
	std::map<std::string,std::string> columes)
{
	std::string str = "INSERT INTO ";
	str += table;
	str += " ( ";

	for (std::map<std::string, std::string>::iterator it = columes.begin();
		it != columes.end();
		it++)
	{
		if (it != columes.begin())
			str += ",";

		str += "\'";
		str += it->first.c_str();
		str += "\'";
	}

	str += " ) ";

	str += "VALUES ( ";

	for (std::map<std::string, std::string>::iterator it = columes.begin();
		it != columes.end();
		it++)
	{
		if (it != columes.begin())
			str += ",";

		str += "\'";
		str += it->second.c_str();
		str += "\'";
	}
	str += " );";
	return str;
}

void SqliteWritter::test()
{
#define FILENAME		"process.db"
#define TABLENAME		"process"

	char * err = NULL;  
	int ret = 0;  
	sqlite3 * db = NULL;  

	ret = sqlite3_open(FILENAME, &db);  
	if ( ret != SQLITE_OK )  
	{  
		printf("Could not open database: %s", sqlite3_errmsg(db));  
		return;  
	}  
 
	std::map<std::string, std::string> columns;
	columns["Name"]							= "varchar[1023]";
	columns["Instance"]						= "int";
	columns["WorkingSet"]					= "varchar[1023]";
	columns["Working Set"]					= "varchar[1023]";
	columns["Working Set Peak"]				= "varchar[1023]";
	columns["Private Bytes"]				= "varchar[1023]";
	columns["Private Bytes Peak"]			= "varchar[1023]";
	columns["Virtual Bytes"]				= "varchar[1023]";
	columns["Virtual Bytes Peak"]			= "varchar[1023]";
	columns["Page File Bytes"]				= "varchar[1023]";
	columns["Pool Paged Bytes"]				= "varchar[1023]";
	columns["Pool Nonpaged Bytes"]			= "varchar[1023]";
	columns["Thread Count"]					= "int";
	columns["ID Process"]					= "int";
	columns["Handle Count"]					= "int";
	columns["Creating Process ID"]			= "int";
	columns["Priority Base"]				= "int";
	columns["Elapsed Time"]					= "varchar[1023]";
	columns["% Processor Time"]				= "int";
	columns["% User Time"]					= "int";
	columns["% Privileged Time"]			= "int";
	columns["IO Read Operations/sec"]		= "int";
	columns["IO Write Operations/sec"]		= "int";
	columns["IO Data Operations/sec"]		= "int";
	columns["IO Other Operations/sec"]		= "int";
	columns["IO Read Bytes/sec"]			= "int";
	columns["IO Write Bytes/sec"]			= "int";
	columns["IO Data Bytes/sec"]			= "int";
	columns["IO Other Bytes/sec"]			= "int";


	std::string str = GetCreateTableString(TABLENAME,columns);
	sqlite3_exec( db, str.c_str(), _callback_exec, 0, &err );  
	if ( ret != SQLITE_OK )  
	{  
		printf("SQL error: %s\n", err);  
		sqlite3_free(err);  
	}  

	columns.clear();
	columns["Name"]							= "null";
	columns["Instance"]						= "0";
	columns["WorkingSet"]					= "null";
	columns["Working Set"]					= "null";
	columns["Working Set Peak"]				= "null";
	columns["Private Bytes"]				= "null";
	columns["Private Bytes Peak"]			= "null";
	columns["Virtual Bytes"]				= "null";
	columns["Virtual Bytes Peak"]			= "null";
	columns["Page File Bytes"]				= "null";
	columns["Pool Paged Bytes"]				= "null";
	columns["Pool Nonpaged Bytes"]			= "null";
	columns["Thread Count"]					= "0";
	columns["ID Process"]					= "0";
	columns["Handle Count"]					= "0";
	columns["Creating Process ID"]			= "0";
	columns["Priority Base"]				= "0";
	columns["Elapsed Time"]					= "null";
	columns["% Processor Time"]				= "0";
	columns["% User Time"]					= "0";
	columns["% Privileged Time"]			= "0";
	columns["IO Read Operations/sec"]		= "0";
	columns["IO Write Operations/sec"]		= "0";
	columns["IO Data Operations/sec"]		= "0";
	columns["IO Other Operations/sec"]		= "0";
	columns["IO Read Bytes/sec"]			= "0";
	columns["IO Write Bytes/sec"]			= "0";
	columns["IO Data Bytes/sec"]			= "0";
	columns["IO Other Bytes/sec"]			= "0";

	str = GetInsertString(TABLENAME,columns);
	sqlite3_exec( db, str.c_str(), _callback_exec, 0, &err );  
	if ( ret != SQLITE_OK )  
	{  
		printf("SQL error: %s\n", err);  
		sqlite3_free(err);  
	}

	str = GetSelectAllString(TABLENAME);
	sqlite3_exec( db, str.c_str(), _callback_exec, 0, &err );  
	if ( ret != SQLITE_OK )  
	{  
		printf("SQL error: %s\n", err);  
		sqlite3_free(err);  
	}

	sqlite3_close(db);  
	db = NULL;  

	return; 
}

