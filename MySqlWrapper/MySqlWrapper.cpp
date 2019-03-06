
#include "MySqlWrapper.h" 
   
BEGIN_MODULE_NAMESPACE

MySQLWrapper * MySQLWrapper::instance = NULL;

//���캯�� ��ʼ���������������� 
MySQLWrapper::MySQLWrapper(): 
    errorNum(0),errorInfo("ok") 
{ 
    mysql_library_init(0,NULL,NULL); 
    mysql_init(&mysqlInstance); 
    mysql_options(&mysqlInstance,MYSQL_SET_CHARSET_NAME,"gbk"); 
} 
   
MySQLWrapper::~MySQLWrapper() 
{ 
   
} 

MySQLWrapper * MySQLWrapper::GetInstance()
{
	if ( instance==NULL )
		return new MySQLWrapper();
	return instance;
}

//����MySQL 
bool MySQLWrapper::Connet(char* server, char* username, char* password, char* database,int port) 
{ 
    if(mysql_real_connect(&mysqlInstance,server,username,password,database,port,0,CLIENT_MULTI_STATEMENTS) != NULL) 
	{
		strcpy( _server, server );
		strcpy( _username, username );
		strcpy( _password, password );
		strcpy( _database, database );
		_port = port;

		my_bool bReconnect = true;
		mysql_options( &mysqlInstance, MYSQL_OPT_RECONNECT, &bReconnect );

        return true; 
	}
    else 
        RunFailed();

    return false; 
} 

//�ж����ݿ��Ƿ���ڣ��������򴴽����ݿ⣬���� 
bool MySQLWrapper::CreateDatabase(std::string& dbname) 
{ 
    std::string queryStr = "create database if not exists "; 
    queryStr += dbname; 
    if (0 == mysql_query(&mysqlInstance,queryStr.c_str())) 
    { 
        queryStr = "use "; 
        queryStr += dbname; 
        if (0 == mysql_query(&mysqlInstance,queryStr.c_str())) 
        { 
            return true; 
        } 
           
    } 
    RunFailed(); 
    return false; 
} 

//�ж����ݿ����Ƿ������Ӧ���������򴴽��� 
bool MySQLWrapper::CreateTable(const std::string& query) 
{ 
    if (0 == mysql_query(&mysqlInstance,query.c_str())) 
    { 
        return true; 
    } 
    RunFailed(); 
    return false; 
} 
   
//д������ 
bool MySQLWrapper::ExecuteSQL( std::string strSql ) 
{ 
	if(0==mysql_query(&mysqlInstance,strSql.c_str())) 
	{
        return true; 
	}
    else 
        RunFailed(); 
    return false;    
} 

//��ȡ���� 
bool MySQLWrapper::FetchData(std::string queryStr, std::vector<std::vector<std::string> >& data) 
{ 
    if(0!=mysql_query(&mysqlInstance,queryStr.c_str())) 
    { 
        RunFailed(); 
        return false; 
    } 
   
    result=mysql_store_result(&mysqlInstance); 
   
    int row=mysql_num_rows(result); 
    int field=mysql_num_fields(result); 
   
    MYSQL_ROW line=NULL; 
    line=mysql_fetch_row(result); 
   
    int j=0; 
    std::string temp; 
    while(NULL!=line) 
    {    
        std::vector<std::string> linedata; 
        for(int i=0; i<field;i++) 
        { 
            if(line[i]) 
            { 
                temp = line[i]; 
                linedata.push_back(temp); 
            } 
            else 
            { 
                temp = ""; 
                linedata.push_back(temp); 
            } 
        } 
        line=mysql_fetch_row(result); 
        data.push_back(linedata); 
    } 
	mysql_free_result(result);

    return true; 
} 
   
//������Ϣ 
void MySQLWrapper::RunFailed() 
{ 
    errorNum=mysql_errno(&mysqlInstance); 
    errorInfo=mysql_error(&mysqlInstance); 

	printf( "ERROR:�������ݿ�ʧ��!errorNum:%d errorInfo:%s\n", errorNum, errorInfo);
	//Connet( _server, _username, _password, _database, _port );
} 

void MySQLWrapper::GetLastError( int &nErrorCode, std::string &errorMsg )
{
	nErrorCode = errorNum;
	if ( errorNum )
	{
		errorMsg = std::string( errorInfo );
	}
}

//�Ͽ����� 
void MySQLWrapper::Close() 
{ 
    mysql_close(&mysqlInstance); 
}

int MySQLWrapper::AffectedRows()
{
	return mysql_affected_rows( &mysqlInstance );
}

END_MODULE_NAMESPACE
