#pragma once

#ifndef __MYSQL_INTERFACE_H__ 
#define __MYSQL_INTERFACE_H__ 

#ifdef _WIN32   
#include <winsock.h>
#endif 
#include <iostream> 
#include <string> 
#include <vector> 
#include <string> 
#include <string.h>
#include <stdio.h>
  
#include "fspcommon.h"
#ifdef _WIN32
#include "mysql.h" 
#else
#include "mysql.h"
#endif

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "libmysql.lib") 
#endif
BEGIN_MODULE_NAMESPACE 

class FUANDA_DBACCESS_EXPORT MySQLWrapper 
{ 
public:   
    MySQLWrapper(); 
    virtual ~MySQLWrapper(); 
   
	static MySQLWrapper * GetInstance();
    bool Connet(char* server, char* username, char* password, char* database,int port); 
    void Close(); 
   
	bool CreateDatabase(std::string& dbname); 
    bool CreateTable(const std::string& query); 
   
    bool ExecuteSQL( std::string strSql ); 
    bool FetchData(std::string queryStr, std::vector<std::vector<std::string> >& data); 

    void RunFailed(); 
	void GetLastError( int &nErrorCode, std::string &errorMsg );

	int AffectedRows();

public: 
    int errorNum;                    //错误代号 
    const char* errorInfo;             //错误提示 
   
private: 
    MYSQL mysqlInstance;                      //MySQL对象，必备的一个数据结构 
    MYSQL_RES *result;                 //用于存放结果 建议用char* 数组将此结果转存 

	static MySQLWrapper *instance;
	char _server[16];
	char _username[32];
	char _password[32];
	char _database[32];
	int _port;
}; 

END_MODULE_NAMESPACE

#endif

