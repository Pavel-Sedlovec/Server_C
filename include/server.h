#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <libpq-fe.h>
#include "http.h"

extern int count_message;
extern pthread_mutex_t mutex;

void* start_client(void* arg);
void creat_pthread(int client_sockfd);

int specific_route(HTTPrequest* req, PGconn* conn, int client_sockfd);
void universal_route(HTTPrequest* req, PGconn* conn, int client_sockfd);
void GET_request(PGconn* conn, int client_sockfd, char* table_name, int* id);
void DELETE_request(PGconn* conn, int client_sockfd, char* table_name, int* id);
void POST_request(PGconn* conn, int client_sockfd, char* table_name, HTTPrequest* req);
#endif