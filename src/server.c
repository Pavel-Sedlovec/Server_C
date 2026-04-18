#include "server.h"
#include "db.h"
#include "http.h"
#include "cJSON.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <libpq-fe.h>

static char HELLO[] = "Super krutoi server";


/*
* Создание нового потока для пришедшего пользователя
* @param arg данные для нового потока
*/
void* start_client(void* arg){
    pthread_detach(pthread_self());
    int client_sockfd = *((int*)arg);
    free(arg);

    PGconn *conn = connect_db();
    if (!conn) {
        close(client_sockfd);
        return NULL;
    }

    char buffer[2048];
    int bytes_read;
    HTTPrequest req;

    while((bytes_read = recv(client_sockfd, buffer, sizeof(buffer) - 1, 0)) > 0){
        buffer[bytes_read] = '\0';
        printf("Client sent request\n");
        parse_req_http(&req, buffer);

        printf("Method: %s, Path: %s\n", req.method, req.path);   

        if(req.path && strncmp(req.path, "/api/", 5) == 0){
            char *table_name = req.path + 5;
            char *slash = strchr(table_name, '/');
            int id_val;
            int* id_ptr = NULL;

            if(slash){
                *slash = '\0';
                id_val = atoi(slash + 1);
                id_ptr = &id_val;
            }

            if(strcmp(req.method, "GET") == 0){
                GET_request(conn, client_sockfd, table_name, id_ptr);
            }
            else if (strcmp(req.method, "DELETE") == 0){ 
                DELETE_request(conn, client_sockfd, table_name, id_ptr);               
            }
            else if (strcmp(req.method, "POST") == 0){
                POST_request(conn, client_sockfd, table_name, req);
            }

        }else{
            send_response(client_sockfd, 404, "text/plain", "Not Found");
        }

        memset(buffer, 0, sizeof(buffer));
        
        pthread_mutex_lock(&mutex);
        count_message++;
        pthread_mutex_unlock(&mutex);
    }

    close_db(conn);
    close(client_sockfd);
    return NULL;
}

void GET_request(PGconn* conn, int client_sockfd, char* table_name, int* id){
    char* json = NULL;

    if(id == NULL){
        json = get_all_json(conn, table_name);
    }
    else{
        json = get_byId(conn, table_name, id); //Еще не реализовано
    }

    if(json != NULL){
        send_response(client_sockfd, 200, "application/json", json);
        free(json);
    }
    else {
        send_response(client_sockfd, 404, "application/json", "{\"error\":\"Table not found or DB error\"}");
    }
}

void DELETE_request(PGconn* conn, int client_sockfd, char* table_name, int* id){
    if(id != NULL){
        if (delete_record(conn, table_name, id)) {
            send_response(client_sockfd, 200, "application/json", "{\"status\":\"ok\"}");
        }
    }
}

void POST_request(PGconn* conn, int client_sockfd, char* table_name, HTTPrequest* req){
    cJSON *root = cJSON_Parse(req.body);

    if(root != NULL){
        if(post_record(conn, table_name, root)){
            send_response(client_sockfd, 201, "application/json", "{\"status\":\"created\"}");
        }
        else{
            send_response(client_sockfd, 500, "application/json", "{\"error\":\"db insert error\"}");
        }
    }
    else{
        send_response(client_sockfd, 400, "application/json", "{\"error\":\"invalide json\"}");
    }

    cJSON_Delete(root);
}


void creat_pthread(int client_sockfd){
    int *new_client_sockfd = malloc(sizeof(int));
    *new_client_sockfd = client_sockfd;
    pthread_t thread;

    if(pthread_create(&thread, NULL, start_client, (void*)new_client_sockfd) != 0){
        perror("Error creat flow");
        free(new_client_sockfd);
    }
}