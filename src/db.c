#include "db.h"
#include <stdio.h>

/*
* Функция для подключения к БД
* @return Дескриптор подключения PGconn*. Если не смогли подключится - NULL
*/
PGconn* connect_db(){
    const char *conninfo = "host=localhost port=5432 dbname=mydb user=myuser password=mypass";
    PGconn *conn;
    conn = PQconnectdb(conninfo);
    if(PQstatus(conn) != CONNECTION_OK){
        printf("Error connection DB");
        close_db(conn);
        return NULL;       
    }
    printf("DB connect");
    return conn;
}

void close_db(PGconn* conn){
    if(conn){
        PQfinish(conn);
        printf("Connection close");
    }
}