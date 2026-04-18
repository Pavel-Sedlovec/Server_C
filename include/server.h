#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <libpq-fe.h>

extern int count_message;
extern pthread_mutex_t mutex;

void* start_client(void* arg);
void creat_pthread(int client_sockfd);

void GET_request(PGconn* conn, int client_sockfd, char* table_name, int* id);
void DELETE_request(PGconn* conn, int client_sockfd, char* table_name, int* id);
void POST_request(PGconn* conn, int client_sockfd, char* table_name, HTTPrequest* req);
#endif