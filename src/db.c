#include "db.h"
#include <stdio.h>
#include <string.h>

/*
* Функция для подключения к БД
* @return Дескриптор подключения PGconn*. Если не смогли подключится - NULL
*/
PGconn* connect_db(){
    char conninfo[1024] = "///";

    // const char *host = getenv("DB_HOST");
    // const char *port = getenv("DB_PORT");
    // const char *name = getenv("DB_NAME");
    // const char *user = getenv("DB_USER");
    // const char *pass = getenv("DB_PASS");
    // if (!host || !port || !name || !user || !pass) {
    //     fprintf(stderr, "Error: One or more DB environment variables are missing!\n");
    //     return NULL;
    // }
    //int f = snprintf(conninfo, sizeof(conninfo), 
         //"host=%s port=%s dbname=%s user=%s password=%s",
         //getenv("DB_HOST"), getenv("DB_PORT"), getenv("DB_NAME"), getenv("DB_USER"), getenv("DB_PASS"));
    
    // if(f < 0){
    //     perror("PIZZZZZDA");
    //     exit(0);
    // }
    PGconn *conn;
    conn = PQconnectdb(conninfo);
    if(PQstatus(conn) != CONNECTION_OK){
        //perror("Error connection DB");
        fprintf(stderr, "Ошибка подключения: %s\n", PQerrorMessage(conn));
        close_db(conn);
        return NULL;       
    }
    fprintf(stdout, "DB connect");
    return conn;
}

int get_all_courses(PGconn *conn, Categories *dest, int size){
    
    PGresult *res = PQexec(conn, "SELECT id, title FROM categories");

    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        fprintf(stderr, "request failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }
    int rows = PQntuples(res);

    int count = size;
    if(rows < size) count = rows;

    for(int i = 0; i < count; i++){
        
        dest[i].id = atoi(PQgetvalue(res, i, 0));
        strncpy(dest[i].categories, PQgetvalue(res, i, 1), sizeof(dest[i].categories));
        dest[i].categories[sizeof(dest[i].categories) - 1] = '\0';
    }
    PQclear(res);
    return count;
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

    char line[64];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;

        char *divider = strchr(line, '=');

        if (divider != NULL && divider != line) {
            
            *divider = '\0';   
            char *key = line;    
            char *value = divider + 1; 

            if (setenv(key, value, 1) != 0) {
                perror("setenv error");
            }
        }
    }
    fclose(file);
}