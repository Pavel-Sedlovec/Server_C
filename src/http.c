#include <stdio.h>
#include "http.h"

void parse_req_http(HTTPrequest *req, char *buffer) {
    sscanf(buffer, "%s %s", req->method, req->path);    
}

void send_response(int conn, int status, const char *type, const char *body){
    char responce[2048] = {0};
    sprintf(responce, 
    "HTTP/1.1 %d OK\r\n"
    "Content-Type: %s; charset=UTF-8\r\n"
    "Content-Length: %ld\r\n"
    "\r\n", 
    status, type, strlen(body));

    send(conn, responce, strlen(responce), 0);
    send(conn, body, strlen(body), 0);

}
