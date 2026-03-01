#include "server.h"
#include "db.h"
#include "signals.h"
#include "net.h"
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

static volatile int _flagStop = 0;
int count_message = 0;
pthread_mutex_t mutex;

void server_stop(int sig){
    _flagStop = 1;
}

int main(){
    
    int sockfd = creat_server_socked(1130, 5);

    setup_signals(server_stop);
    struct sockaddr_in client_addr = {0};   
    socklen_t addrlen = sizeof(client_addr);
    
    pthread_mutex_init(&mutex, NULL);
    load_env();
    
    printf("Server start\n");

    while(!_flagStop){
        int client_sockfd = Accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
        if(client_sockfd == -1){            
                continue;       
        }
        printf(">> Accepted new connection. IP: %s\n", inet_ntoa(client_addr.sin_addr));
        
        creat_pthread(client_sockfd);
    }
    close(sockfd);
    return 0;    
}