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

void readbuffer(int socket, int fid, char *ipaddr)
{
	char socbuf[255];
	int size = 0;
	char buffer[255];
	int count = 0;
	char mobile[25];
	char *startPos = NULL;

	memset(mobile, 0, sizeof(mobile));

	size = read(socket, socbuf, 255);

	if (size <= 0)
        return;
    int i = 0;
    bool registered = false;

    startPos = strstr(socbuf, "MobileNo:");
    if (startPos == NULL)
        return;

    startPos += 9;
    while(*startPos != '\n' && *startPos != '\0')
    {
        mobile[i++] = *startPos;
        startPos++;
    }

	while (1)
	{
		memset(socbuf, 0, sizeof(socbuf));
		memset(buffer, 0, sizeof(buffer));
        REQUESTED_INFO *requested;
        int rows = 0;

		count++;
		if (registered == true)
        {
            sprintf(buffer, "SELECT * FROM REGISTERED_USER WHERE MOBILE='%s'", mobile);

            if (!ExecuteQuery(buffer))
            {
                sleep(10);
                continue;
            }
            requested = ExecuteSelectRows(&rows);

            if ( rows == 0 )
            {
                sprintf(buffer, "INSERT INTO REGISTERED_USER(mobile) VALUES('%s')", mobile);
                ExecuteQuery(buffer);
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "NOT_INSTALLED : %s", mobile);
                write(socket, buffer, strlen(buffer));
                registered = true;
                sleep(5);
            }
        }
        else
        {
            sprintf(buffer, "SELECT * FROM INFO WHERE MOBILE='%s'", mobile);

            if (!ExecuteQuery(buffer))
            {
                sleep(10);
                continue;
            }
            requested = ExecuteSelectRows(&rows);

            if ( rows > 0 )
            {
                for ( int i = 0; i < rows; i++)
                {
                    sprintf(buffer, "Name:%s\nMobileno:%s\nEmail:%s\n", requested[i].name, requested[i].mobile, requested[i].email);
                    write(socket, buffer, strlen(buffer));

    //                write(fid, buffer, strlen(buffer));
                }
                sprintf(buffer, "DELETE FROM INFO WHERE MOBILE='%s'", mobile);
                ExecuteQuery(buffer);
            }
        }
		sleep(5);
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

	fid = open("./readlog.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
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
	addr.sin_port = htons(5060);

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
			readbuffer(connfd, fid, inet_ntoa(cli_addr.sin_addr));
			exit(0);
		}
		else
			close(connfd);
	}
	close(fid);

	return true;
}
