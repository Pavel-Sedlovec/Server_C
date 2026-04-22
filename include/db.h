#ifndef DB_H
#define DB_H

#include <libpq-fe.h>
#include "http.h"
#include "cJSON.h"

PGconn* connect_db();
void close_db(PGconn* conn);

char* get_all_json(PGconn *conn, char *table_name);
char* get_byId(PGconn *conn, char *table_name, int id);
char* get_courses_by_category(PGconn *conn, int category_id);
char* get_course_modules(PGconn *conn, int course_id);
char* get_module_lessons(PGconn *conn, int module_id);
char* get_student_courses(PGconn *conn, int student_id);

int delete_record(PGconn* conn, char *table_name, int id);
int post_record(PGconn* conn, const char *table_name, cJSON *json_obj);
int enroll_student(PGconn* conn, int student_id, int course_id);

void load_env();

typedef struct
{
    int id;
    char categories[64];
}Categories;

#endif