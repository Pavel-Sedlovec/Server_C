/*
* Определение структур и функций для работы с БД
*/
#ifndef DB_H
#define DB_H

#include <libpq-fe.h>

PGconn* connect_db();
void close_db(PGconn* conn);

void load_env();

typedef struct
{
    int id;
    char categories[64];
}Categories;


#endif