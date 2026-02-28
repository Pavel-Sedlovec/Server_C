#include "server.h"
#include "db.h"
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
    
    PGconn *conn = connect_db();
    // В дальнейшем проверка на NULL

    char buffer[256];
    int bytes_read;
    int client_sockfd = *((int*)arg);
    free(arg);

    send(client_sockfd, HELLO, strlen(HELLO), 0); // Отправляем приветствие

    while((bytes_read = recv(client_sockfd, buffer, sizeof(buffer), 0)) > 0){
        printf("Client cent: %.*s\n", bytes_read, buffer);

        if(strncmp(buffer, "/STAT", 5) == 0){
            sprintf(buffer, "count message: %d", count_message);
            send(client_sockfd, buffer, strlen(buffer), 0);            
        }else{
            send(client_sockfd, buffer, bytes_read, 0);
        }

        memset(buffer, 0, sizeof(buffer));

        pthread_mutex_lock(&mutex);
        count_message = count_message + 1;
        pthread_mutex_unlock(&mutex);
    }
    close(client_sockfd);
    close_db(conn);
    return NULL;
}