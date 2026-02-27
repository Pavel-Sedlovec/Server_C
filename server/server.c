#include<stdio.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <libpq-fe.h>

static int _flagStop = 0;
static int count_message = 0;
static char HELLO[] = "Super krutoi server";

pthread_mutex_t mutex;

void server_stop(int sig){
    _flagStop = 1;
}

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
        PQfinish(conn);
        return NULL;       
    }
    printf("DB connect");
    return conn;
}

/*
* Создание нового потока для пришедшего пользователя
* @param arg данные для нового потока
*/
void* start_client(void* arg){
    pthread_detach(pthread_self());
    char buffer[256];
    int bytes_read;
    int client_sockfd = *((int*)arg);
    free(arg);
    //send(client_sockfd, HELLO, strlen(HELLO), 0);            
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
    return NULL;
}

int main(){
    int sockfd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t addrlen = sizeof(client_addr);
    struct sigaction sa;
    pthread_mutex_init(&mutex, NULL);

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = server_stop;
    sigaction(SIGINT, &sa, NULL);
    if((sockfd = socket(AF_INET,SOCK_STREAM, 0)) < 0){
        perror("Error socket");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1130);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        perror("Error bind\n");
        return -1;
    }

    listen(sockfd, 5);
    printf("Server\n");

    while(!_flagStop){
        int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
        if(client_sockfd < 0){
            if(errno == EINTR){
                continue;
            }else{
                printf("Error accept\n");
                break;
            }            
        }
        printf(">> Accepted new connection. IP: %s\n", inet_ntoa(client_addr.sin_addr));
        
        int *new_client_sockfd = malloc(sizeof(int));
        *new_client_sockfd = client_sockfd;
        pthread_t thread;
        
        if(pthread_create(&thread, NULL, start_client, (void*)new_client_sockfd) != 0){
            perror("Error creat flow");
            free(new_client_sockfd);
        }                   
    }
    close(sockfd);
    return 0;    
}
