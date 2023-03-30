#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int host_switch(char *server_host);

int server_connect(char *server_host,int port,int *connect_fd)
{
	int                 rc=0;
	int                 rv=-1;
	int                 fd=-1;
	char                buf[128];
	socklen_t           addrlen=sizeof(struct sockaddr_in);
	struct sockaddr_in  server_addr;
	struct addrinfo     *result=NULL;
	struct addrinfo     hints={AI_CANONNAME,AF_INET,SOCK_STREAM,IPPROTO_TCP,0,NULL,NULL,NULL};

	memset(&server_addr,0,sizeof(server_addr));

	rv=host_switch(server_host);
	if(rv==0)
	{
		inet_aton(server_host,&server_addr.sin_addr);
		printf("sever_host:%s\n",server_host);
	}

	if(rv==1)
	{   
		rc=getaddrinfo(server_host,NULL,&hints,&result);
		if(0 == rc) 
		{   
			struct addrinfo     *res=result;
			while(res)
			{   
				server_addr.sin_addr=((struct sockaddr_in *)res->ai_addr)->sin_addr;
				res=res->ai_next;
				break;
			}
			freeaddrinfo(result);
		}
	}
	
	if((*connect_fd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("socket failure:%s\n",strerror(errno));
		rv=-1;
	}
	printf("Create socket[%d] successfully\n\n",*connect_fd);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	if(connect((int)*connect_fd,(struct sockaddr *)&server_addr,addrlen)<0)
	{
		printf("Connect failure:%s\n",strerror(errno));
		rv=-2;
	}

	return rv;
}

int host_switch(char *server_host)
{
	struct in_addr IP={0};
	if(inet_aton(server_host,&IP))
	{
		return 0;
	}
	return 1;
}
