#include "db.h"
#include <stdio.h>

/*
* Функция для подключения к БД
* @return Дескриптор подключения PGconn*. Если не смогли подключится - NULL
*/
PGconn* connect_db(){
    const char *conninfo[1024];
    snprintf(conninfo, sizeof(conninfo), 
         "host=%s port=%s dbname=%s user=%s password=%s",
         getnv("DB_HOST"), getnv("DB_PORT"), getnv("DB_NAME"), getnv("DB_USER"), getnv("DB_PASS"));         
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

/*
* Закрытие соединения с БД
* @param conn дескриптор открытой БД
*/
void close_db(PGconn* conn){
    if(conn){
        PQfinish(conn);
        printf("Connection close");
    }
}

/*
* Функция для загрузки .env файла.
* После выполнения создаются новые переменные окружения с именем ключей из .env
*/
void load_env() {
    char filename[] = ".env";
    FILE *file = fopen(filename, "r");
    if (!file) return;

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line"=");
        char *value = strtok(NULL"\n");
        if (key && value) {
            setenv(key, value, 1);
        }
    }
    fclose(file);
}