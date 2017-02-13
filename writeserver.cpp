#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<sys/stat.h>
#include <arpa/inet.h>

#include "db.h"

void InsertIntoTable(char *buffer, int lenght, char *ipaddr)
{
    char *temp = buffer;
    char *startPos = NULL;
    char *endPos = NULL;
    char name[256];
    char mobile[25];
    char email[255];
    int i = 0;
    char record[1024];

    memset(name, 0, sizeof(name));
    memset(mobile, 0, sizeof(mobile));
    memset(email, 0, sizeof(email));

    startPos = strstr(buffer, "Name:");
    if ( startPos != NULL)
    {
        startPos += 5;
        while(*startPos != '\n')
        {
            name[i++] = *startPos;
            startPos++;
        }
    }
    i = 0;
    startPos = strstr(buffer, "MobileNo:");
    if (startPos != NULL)
    {
        startPos += 9;
        while(*startPos != '\n')
        {
            mobile[i++] = *startPos;
            startPos++;
        }
    }
    i = 0;
    startPos = strstr(buffer, "Email:");
    if (startPos != NULL)
    {
        startPos += 6;
        while(*startPos != '\n')
        {
            email[i++] = *startPos;
            startPos++;
        }
    }

    memset(record, 0, sizeof(record));
    sprintf(record, "SELECT * FROM INFO WHERE MOBILE='%s'", mobile);

    if (!ExecuteQuery(record))
    {
        sleep(10);
        continue;
    }
    requested = ExecuteSelectRows(&rows);
    if ( rows == 0 )
    {
        sprintf(record, "INSERT INTO INFO(Name, email, mobile, ipaddr) VALUES('%s', '%s', '%s', '%s')", name, email, mobile, ipaddr);
        ExecuteQuery(record);

        return true;
    }
    return false;
}

void writebuffer(int socket, int fid, char *ipaddr)
{
	char socbuf[1024];
	int size = 0;
	char buffer[1024];
	int count = 0;

	while (1)
	{
		memset(socbuf, 0, sizeof(socbuf));
		memset(buffer, 0, sizeof(buffer));
		size = read(socket, socbuf, 255);

		if (size <= 0)
        {
            sprintf(buffer, "Read Error, close socket\n");
            write(fid, buffer, strlen(buffer));
            return;
        }
        InsertIntoTable(socbuf, strlen(socbuf), ipaddr);
		sprintf(buffer, "size = %d \t ************ buffer == %sipaddress = %s\n", size, socbuf, ipaddr);
		write(fid, buffer, strlen(buffer));

		memset(buffer, 0, sizeof(buffer));
		sprintf(buffer, "RESPONSE : {%s}\n", socbuf);
		write(socket, buffer, strlen(buffer));
	}
}

int main()
{
	pid_t pid, sid, cpid;
	int fid = -1;
	int listenfd = 0;
	int connfd = 0;
	struct sockaddr_in addr, cli_addr;
	socklen_t  length = 0;

	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid > 0)
		exit(EXIT_SUCCESS);

	fid = open("./writelog.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (fid <= 0)
		exit(EXIT_FAILURE);

	umask(0);
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);
	if(chdir("/") < 0)
		exit(EXIT_FAILURE);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	InitializeSQLTable();
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(5050);

	if ( bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		write(fid, "bind error\n", 11);
		exit(EXIT_FAILURE);
	}
	listen(listenfd, 10);

	while (1)
	{
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &length);

		if (connfd < 0)
		{
			write(fid, "error\n", 6);
			exit(EXIT_FAILURE);
		}

		cpid = fork();

		if (cpid < 0)
		{
			exit(EXIT_FAILURE);
		}

		if (cpid == 0)
		{
			close(listenfd);
			writebuffer(connfd, fid, inet_ntoa(cli_addr.sin_addr));
			exit(0);
		}
		else
			close(connfd);
	}
	close(fid);
}
