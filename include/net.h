#ifndef NET_H
#define NET_H

#include <sys/types.h>
#include <sys/socket.h>

int Socket(int domain, int type, int protocol);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int creat_server_socked(int port, int backlog);

#endif
