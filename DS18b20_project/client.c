#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <getopt.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>
#include "DS18b20.h"
#include "server_connect.h"
#include "databack.h"

#define Temp      "temperature"
#define msg_size  512


void print_usage(char *program);
void socket_signal(int signum);

int  g_sigstop=0;

int main(int argc,char **argv)
{
	int                 connect_fd=-1;
	int                 port;
	int                 rv=0;
	int                 interval_t;
	int                 ch;
	int                 idx;
	int                 num;
	int                 i=0;
	int                 j=0;
	int                 index=0;
	int                 ref[512]={0};

	float               temp=0;
	time_t              t;
	struct tm           *st;

	sql_msg             SQL_MSG;
	struct sockaddr_in  server_addr;
	char                *server_host=NULL;
	char                time_msg[msg_size];
	char                message[msg_size];
	char                server_msg[msg_size];

	struct option       long_options[]={
		                {"host",required_argument,NULL,'H' },
						{"port",required_argument,NULL,'p' },
						{"help",no_argument,NULL,'h' },
						{"time",required_argument,NULL,'t' },
						{"num",required_argument,NULL,'n'},
						{0,0,0,0}
	};

	while((ch=getopt_long(argc,argv,"H:p:t:hn:",long_options,&idx)) != -1)
	{
		switch(ch)
		{
			case 'H':
				server_host=optarg;
				break;
			case 'p':
				port=atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				break;
			case 't':
				interval_t=atoi(optarg);
				break;
			case 'n':
				num=atoi(optarg);
				break;
		}
	}

	rv=server_connect(server_host,port,&connect_fd);

	if(rv < 0)
	{
		printf("Connect server failure in:%d\n",rv);
	}

	signal(SIGINT,socket_signal);
	signal(SIGPIPE,socket_signal);
	
	while(!g_sigstop)
	{
		rv=DS_temp(&temp);
		time(&t);
		st=localtime(&t);
		snprintf(time_msg,64,"%02d.%02d %02d:%02d:%02d",st->tm_mon+1,st->tm_mday,st->tm_hour,st->tm_min,st->tm_sec);
		snprintf(message,sizeof(message),"client_%d  %s :get temperature[%f]",num,time_msg,temp);
		printf("%s\n",message);

		rv = sql_init("temp_info.db",&SQL_MSG);


		for(i=1;;i++)
		{
			if(ref[i] == 0)
			{
				rv = sql_insert(i,&SQL_MSG,"%s",message);
				ref[i]=1;
				break;
			}
		}
		rv = sql_insert(7,&SQL_MSG,"%s",message);


	    rv = sql_select(&SQL_MSG);
		printf("hang:%d,lie:%d\n",SQL_MSG.hang,SQL_MSG.lie);
		index=SQL_MSG.lie;
		for(i=0;i<SQL_MSG.hang;i++)
		{
			memset(message,0,msg_size);
			memset(server_msg,0,msg_size);
			for(j=0;j<SQL_MSG.lie;j++)
			{
				snprintf(message,msg_size,"%s ",SQL_MSG.paz_result[index]);
				strncat(server_msg,message,msg_size);
				index++;
			}
			printf("%s\n",server_msg);
			
		}

		rv = sql_delete(&SQL_MSG);

		if(0 != rv || 0 != g_sigstop)
		{
			printf("DS18b20 get failure in the %d step\n",rv);
			break;
		}

		printf("DS18b20 Temperature:%s\n",message);
		rv=write(connect_fd,message,sizeof(message));
		if(rv <= 0)
		{
			printf("Write failure:%s\n",strerror(errno));
			break;
		}
		memset(message,0,sizeof(message));

		rv=read(connect_fd,message,sizeof(message));
		if(rv <= 0)
		{
			printf("Read failure:%s\n",strerror(errno));
			break;
		}
		printf("Read from server:'%s'\n\n",message);
	}
cleanup:
	close(connect_fd);
	return 0;
}


void print_usage(char *program)
{
	printf("%s: usage \n",program);
	printf("-H(--ipaddr):server host:IP or domain name.\n)");
	printf("-p(--port):server port.\n");
	printf("-h(--help):this is help information.\n");
	
}

void socket_signal(int signum)
{
	if(signum == SIGINT)
	{
		printf("SIGINT signal detected\n");
		g_sigstop=1;
		exit(0);
	}
	if(signum == SIGPIPE)
	{
		printf("SIGPIPE signal detected\n");
		g_sigstop=1;
	}
}
