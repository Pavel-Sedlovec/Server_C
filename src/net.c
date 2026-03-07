#include "net.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

/*
* Функция для создания сокета
* @param domain Семейство протоколов создаваемого сокета
*   AF_INET для сетевого протокола IPv4
*   AF_INET6 для IPv6
*   AF_UNIX для локальных сокетов (используя файл)
* @param type Тип. 
*   SOCK_STREAM для TCP, SOCK_DGRAM для UDP
* @param protocol Протокол для канала связи
* @return Возвращает файловый дескриптор. При ошибке выходим
*/
int Socket(int domain, int type, int protocol){
    int res = socket(domain, type, protocol);
    if(res < 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return res;    
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    int res = bind(sockfd, addr, addrlen);
    if (res < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void Listen(int sockfd, int backlog){
    int res = listen(sockfd, backlog);
    if (res == -1){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int res = accept(sockfd, addr, addrlen);
    if (res < 0) {
        if(errno == EINTR){
                return -1;
            }else{
                perror("accept failed");
                exit(EXIT_FAILURE);
            }          
    }
    return res;
}

int creat_server_socked(int port, int backlog){

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1130);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    Listen(sockfd, backlog);
    return sockfd;
}
