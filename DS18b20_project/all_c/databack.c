#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
#include <unistd.h>
#include <time.h>

#define message_size 512

typedef struct sql_msg
{
	int     hang;
	int     lie;
	char    **paz_result;
	char    *errmsg;
	sqlite3 *sql_db;
}sql_msg;

int sql_init(char *filename,sql_msg *sql_msg)
{
	char     create_table[message_size];

	if(SQLITE_OK != sqlite3_open(filename,&sql_msg->sql_db))
	{
		fprintf(stderr,"open %s\n failure:%s\n",filename,sqlite3_errmsg(sql_msg->sql_db));
		strcpy(sql_msg->errmsg,sqlite3_errmsg(sql_msg->sql_db));
		return -1;
	}
	snprintf(create_table,message_size,"create table temp(ref integer primary key,message text);");
	sqlite3_exec(sql_msg->sql_db,create_table,NULL,NULL,&sql_msg->errmsg);
	if(sql_msg->errmsg != NULL)
	{   
		return -1;
	}
}

int sql_select(sql_msg *sql_msg)
{
	char   *select="select * from temp";

	if(SQLITE_OK != sqlite3_get_table( sql_msg->sql_db,select, &sql_msg->paz_result, &sql_msg->hang, &sql_msg->lie, &sql_msg->errmsg))
	{
		fprintf(stderr,"select temp_info failure:%s\n",sql_msg->errmsg);
		return -1;
	}
	if(sql_msg->errmsg != NULL)
	{   
		return -1;
	}
}

int sql_insert(int ref,sql_msg *sql_msg,char *format,...)
{
	char     time_message[64];
	char     message[message_size];
	char     insert_message[message_size];
	va_list  arg_list;
	
	va_start(arg_list,format);

	memset(message,0,message_size);
	memset(insert_message,0,message_size);
	memset(time_message,0,64);


	vsprintf(message,format,arg_list);

	snprintf(insert_message,message_size,"insert into temp values('%d','%s');",ref,message);
	sqlite3_exec(sql_msg->sql_db,insert_message,NULL,NULL,&sql_msg->errmsg);
	if(sql_msg->errmsg != NULL)
	{
		return -1;
	}
}


int sql_delete(sql_msg *sql_msg)
{
	char     delete_message[message_size];

	snprintf(delete_message,message_size,"delete from temp where ref in (select ref from temp limit %d);",sql_msg->hang);
	sqlite3_exec(sql_msg->sql_db,delete_message,NULL,NULL,&sql_msg->errmsg);
	if(sql_msg->errmsg != NULL)
	{   
		return -1;
	}
}
