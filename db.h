#ifndef DB_H
#define DB_H

#include<stdio.h>
#include<mysql/mysql.h>

typedef struct _REQUESTED_INFO
{
    char name[50];
    char email[50];
    char mobile[25];
    char ipaddr[65];
}REQUESTED_INFO;

extern bool ExecuteQuery(char *query);

extern bool InitializeSQLTable();

extern MYSQL* CreateTable(const char *hostname, const char *username, const char *password);

extern REQUESTED_INFO* ExecuteSelectRows(int *rows);
#endif // DB_H
