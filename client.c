#include <stdio.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h> 
#include <arpa/inet.h> 
#include <string.h>

int main(){
    int fd;
    char buffer[256];
    char message[256];
    int bytes_read;
    //const char host[] = "povt-cluster.tstu.tver.ru";
    //const int port = 44109;
    const int port = 1130;
    const char host[] = "127.0.0.1";
    struct sockaddr_in addr;
    struct hostent *ent;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);
    if( addr.sin_addr.s_addr == INADDR_NONE ) {
        if( (ent = gethostbyname(host)) == NULL ) {
            perror("Error");
        } else {
            memcpy(&addr.sin_addr, ent->h_addr_list[0], 
            ent->h_length);
        }
    }

    if((fd = socket(AF_INET,SOCK_STREAM, 0)) < 0){
        perror("Error fd");
        return -1;
    }

    if(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("Error connect");
        close(fd);
        return -1;
    }

    while(1){
        scanf("%s", message);
        if (strcmp(message, "exit") == 0) break;
        if(send(fd, message, strlen(message), 0) < 0){
            perror("Error send");
        }

        if((bytes_read = recv(fd, buffer, sizeof(buffer), 0)) > 0){
            printf("Server cent: %.*s\n", bytes_read, buffer);
        }
    }
    close(fd);
   
}
