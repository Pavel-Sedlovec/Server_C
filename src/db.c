#include "db.h"
#include "cJSON.h"
#include <stdio.h>
#include <string.h>



/*
* Функция для подключения к БД
* @return Дескриптор подключения PGconn*. Если не смогли подключится - NULL
*/
PGconn* connect_db(){
    char conninfo[1024] = "string connect DB";

    // const char *host = getenv("DB_HOST");
    // const char *port = getenv("DB_PORT");
    // const char *name = getenv("DB_NAME");
    // const char *user = getenv("DB_USER");
    // const char *pass = getenv("DB_PASS");
    // if (!host || !port || !name || !user || !pass) {
    //     fprintf(stderr, "Error: One or more DB environment variables are missing!\n");
    //     return NULL;
    // }
    // int f = snprintf(conninfo, sizeof(conninfo), 
    //      "host=%s port=%s dbname=%s user=%s password=%s",
    //      getenv("DB_HOST"), getenv("DB_PORT"), getenv("DB_NAME"), getenv("DB_USER"), getenv("DB_PASS"));
    
    // if(f < 0){
    //     perror("error");
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


char* get_all_json(PGconn *conn, char *table_name){
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM %s", table_name);

    PGresult *res = PQexec(conn, query);

    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        fprintf(stderr, "request failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    int rows = PQntuples(res);
    int cols = PQnfields(res);

    cJSON *array = cJSON_CreateArray();
 
    for (int i = 0; i < rows; i++) {
        cJSON *item = cJSON_CreateObject();
        for (int j = 0; j < cols; j++) {
            cJSON_AddStringToObject(item, PQfname(res, j), PQgetvalue(res, i, j));
        }
        cJSON_AddItemToArray(array, item);
    }

    char *result_str = cJSON_PrintUnformatted(array);
    cJSON_Delete(array);
    PQclear(res);
    return result_str;

}

int delete_record(PGconn* conn, char *table_name, int id){
    char query[256];
    snprintf(query, sizeof(query), "DELETE FROM %s WHERE id = %d", table_name, id);

    PGresult *res = PQexec(conn, query);

    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        fprintf(stderr, "delete failed: %s", PQerrorMessage(conn));
        PQclear(res);        
        return 0;
    }
    PQclear(res);
    return 1;
}

int post_record(PGconn* conn, const char *table_name, cJSON *json_obj) {
    if (json_obj == NULL) return 0;

    char columns[512] = "";
    char values[512] = "";
    cJSON *item = NULL;

    cJSON_ArrayForEach(item, json_obj) {
        strcat(columns, item->string);
        strcat(columns, ", ");

        strcat(values, "'");
        if (cJSON_IsNumber(item)) {
            char num_buf[32];
            snprintf(num_buf, sizeof(num_buf), "%g", item->valuedouble);
            strcat(values, num_buf);
        } else {
            strcat(values, item->valuestring);
        }
        strcat(values, "', ");
    }

    if (strlen(columns) > 2) columns[strlen(columns) - 2] = '\0';
    if (strlen(values) > 2) values[strlen(values) - 2] = '\0';

    char query[1024];
    snprintf(query, sizeof(query), "INSERT INTO %s (%s) VALUES (%s)", table_name, columns, values);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "INSERT failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 0;
    }

    PQclear(res);
    return 1;
}

char* get_byId(PGconn *conn, char *table_name, int id) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM %s WHERE id = %d", table_name, id);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        PQclear(res);
        return NULL;
    }

    cJSON *item = cJSON_CreateObject();
    for (int j = 0; j < PQnfields(res); j++) {
        cJSON_AddStringToObject(item, PQfname(res, j), PQgetvalue(res, 0, j));
    }

    char *result_str = cJSON_PrintUnformatted(item);
    cJSON_Delete(item);
    PQclear(res);
    return result_str;
}

char* get_courses_by_category(PGconn *conn, int category_id){
    char query[256];
    snprintf(query, sizeof(query), 
        "SELECT c.id, c.title, c.description, u.name as author, cat.title as category "
        "FROM courses c "
        "JOIN users u ON c.author_id = u.id "
        "JOIN categories cat ON c.category_id = cat.id "
        "WHERE c.category_id = %d", category_id);

    PGresult *res = PQexec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        fprintf(stderr, "request failed: %s", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    int rows = PQntuples(res);
    int cols = PQnfields(res);
    cJSON *array = cJSON_CreateArray();
 
    for (int i = 0; i < rows; i++) {
        cJSON *item = cJSON_CreateObject();
        for (int j = 0; j < cols; j++) {
            cJSON_AddStringToObject(item, PQfname(res, j), PQgetvalue(res, i, j));
        }
        cJSON_AddItemToArray(array, item);
    }

    char *result_str = cJSON_PrintUnformatted(array);
    cJSON_Delete(array);
    PQclear(res);
    return result_str;
}

int enroll_student(PGconn* conn, int student_id, int course_id){
    char query[256];
    snprintf(query, sizeof(query), 
        "INSERT INTO enrollments (student_id, course_id) VALUES (%d, %d)", 
        student_id, course_id);

    PGresult *res = PQexec(conn, query);
    if(PQresultStatus(res) != PGRES_COMMAND_OK){
        fprintf(stderr, "enroll failed: %s", PQerrorMessage(conn));
        PQclear(res);        
        return 0;
    }
    PQclear(res);
    return 1;
}

char* get_course_modules(PGconn *conn, int course_id){
    char query[256];
    snprintf(query, sizeof(query), 
        "SELECT id, title, ordinal_number FROM modules WHERE course_id = %d ORDER BY ordinal_number", 
        course_id);

    PGresult *res = PQexec(conn, query);
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        PQclear(res);
        return NULL;
    }

    int rows = PQntuples(res);
    int cols = PQnfields(res);
    cJSON *array = cJSON_CreateArray();
 
    for (int i = 0; i < rows; i++) {
        cJSON *item = cJSON_CreateObject();
        for (int j = 0; j < cols; j++) {
            cJSON_AddStringToObject(item, PQfname(res, j), PQgetvalue(res, i, j));
        }
        cJSON_AddItemToArray(array, item);
    }

    char *result_str = cJSON_PrintUnformatted(array);
    cJSON_Delete(array);
    PQclear(res);
    return result_str;
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