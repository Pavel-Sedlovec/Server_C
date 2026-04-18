/*
* Определение структур и функций для работы с БД
*/
#ifndef DB_H
#define DB_H

#include <libpq-fe.h>
#include "http.h"

PGconn* connect_db();
void close_db(PGconn* conn);

char* get_all_json(PGconn *conn, char *table_name);
char* get_byId(conn, table_name, id);

int delete_record(PGconn* conn, char *table_name, int id);

void load_env();

typedef struct
{
    int id;
    char categories[64];
}Categories;


#endif