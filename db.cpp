#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mysql/mysql.h>
#include<string>

#include "db.h"

using namespace std;

static MYSQL *sqlConnection = NULL;

bool ExecuteQuery(char *query)
{
	if(mysql_query(sqlConnection, query))
		return false;

	return true;
}

REQUESTED_INFO* ExecuteSelectRows(int *rows)
{
    MYSQL_ROW       mysqlRow;
    MYSQL_RES      *mysqlResult = NULL;
    REQUESTED_INFO *records = NULL;
    int i = 0;
    my_ulonglong    numRows;


    *rows = 0;
    if (mysqlResult = mysql_store_result(sqlConnection))
    {
        numRows = mysql_num_rows(mysqlResult);
    }
    records = (REQUESTED_INFO*)calloc(sizeof(REQUESTED_INFO), numRows);

    if (records == NULL)
        return NULL;

    while(mysqlRow = mysql_fetch_row(mysqlResult)) // row pointer in the result set
    {
         strcpy(records[i].name, mysqlRow[1] ? mysqlRow[1] : "NULL");
         strcpy(records[i].email, mysqlRow[2] ? mysqlRow[2] : "NULL");
         strcpy(records[i].mobile, mysqlRow[3] ? mysqlRow[3] : "NULL");
         strcpy(records[i].ipaddr, mysqlRow[4] ? mysqlRow[4] : "NULL");
         i++;
    }
    *rows = i;
    if(mysqlResult)
    {
        mysql_free_result(mysqlResult);
        mysqlResult = NULL;
    }

    return records;
}

MYSQL* CreateTable(const char *hostname, const char *username, const char *password)
{
	MYSQL *sqlConn = NULL;
	MYSQL *connection = NULL;

	sqlConn = mysql_init(NULL);

	try
	{
		connection = mysql_real_connect(sqlConn,
						hostname,
						username,
						password,
						NULL,
						0,
						NULL,
						0);
		if(connection == NULL)
			throw -1;
	}
	catch (int error)
	{
		if (error == -1)
			return NULL;
	}

	return sqlConn;
}

bool InitializeSQLTable()
{
	string hostname = "localhost";
	string userid = "root";
	string password = "SUHAIL123";

	sqlConnection = CreateTable(hostname.c_str(), userid.c_str(), password.c_str());
    if ( sqlConnection == NULL )
        return false;

	ExecuteQuery("CREATE DATABASE efeedback");
	ExecuteQuery("USE efeedback");
	ExecuteQuery("CREATE TABLE REGISTERED_USER (Id INT PRIMARY KEY AUTO_INCREMENT, mobile char(50))");
	ExecuteQuery("CREATE TABLE INFO (Id INT PRIMARY KEY AUTO_INCREMENT, Name char(50), email char(50), mobile char(50), ipaddr char(100))");
//	ExecuteQuery(sqlConnection, "INSERT INTO INFO VALUES('Suhail Ahmed', 'suhail.k@lge.com', '9986211139', '192.168.1.4')");

	return true;
}
