#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <wiringPi.h>
#include <arpa/inet.h>

#define MAX 2
#define PORT 9000


char quit[] = "exit"; 

int numClient = 0;

void light(int n);

int main()
{
	int connSock;
	struct sockaddr_in s_addr;
	int  n;
	char serverAddr[] = "52.11.52.152";

	char rcvBuffer[BUFSIZ], sbuf[BUFSIZ];
	int maxfd;

	fd_set read_fds;
	
	connSock = socket(PF_INET, SOCK_STREAM, 0 );
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = inet_addr(serverAddr);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	if(connect(connSock, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1)
	{
		printf("Can not connect\n");
		return -1;
	}

	while(1)
	{
		maxfd = connSock +1;
		FD_ZERO(&read_fds);
		FD_SET(0, &read_fds);
		FD_SET(connSock, &read_fds);

		if( select(maxfd, &read_fds, NULL, NULL, NULL) < 0 )
		{
			printf("select error\n");
			exit(-1);
		}

		printf("client waiting-- \n");

		if(FD_ISSET(0,&read_fds))
		{
			if( ( n = read(0,sbuf,BUFSIZ)) > 0 )
			{
				if( write(connSock, sbuf, n) != n )
					printf("server fail sending\n");
			}
			if( strncmp( sbuf, quit, 4) == 0 )
				break;
		}
		
		if( FD_ISSET(connSock, &read_fds) )
		{
			if( ( n = read(connSock, rcvBuffer, sizeof(rcvBuffer))) != 0 )
			{
				printf("%s",rcvBuffer);

				if( rcvBuffer[0] == 'l' && rcvBuffer[1] == '0')
				{
					light(0);
				}
				else if( rcvBuffer[0] == 'l' && rcvBuffer[1] == '1')
				{
					light(1);
				} // WOL 	
				else if( rcvBuffer[0] == 'p' && rcvBuffer[1] == '1')
				{
					printf("aa");
					char buff[BUFSIZ];
					FILE *fp;
	
					fp = popen("wakeonlan D0:50:99:1D:47:AE","r");

					if( NULL == fp)
						perror("wol fail\n");

					while(fgets(buff,BUFSIZ,fp))
						printf("%s",buff);

					pclose(fp);
				}
			}
			if( strncmp(rcvBuffer,quit,4) == 0 )
				break;
		}
		
	}
	
	return 0;
}

void light(int n)
{
	wiringPiSetup();

	pinMode(7,OUTPUT);

	if( n == 1 )
	{
		digitalWrite(7,1);
		digitalWrite(7,0);
	}
	else
	{
		digitalWrite(7,0);
		digitalWrite(7,1);
	}
	printf("light excute\n");
}

