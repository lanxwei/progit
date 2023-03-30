#include <sqlite3.h>

typedef struct sql_msg
{
	int      hang;
	int      lie;
	char     **paz_result;
	char     *errmsg;
	sqlite3  *sql_db;
}sql_msg;


enum month
{
	Jan,
	Feb,
	Mar,
	APr,
	May,
	Jun,
	Jul,
	Aug,
	Sept,
	Oct,
	Nov,
	Dec
};


int sql_init(char *filename,sql_msg *sql_msg);
int sql_select(sql_msg *sql_msg);
int sql_insert(int ref,sql_msg *sql_msg,char *format,...);
int sql_delete(sql_msg *sql_msg);
