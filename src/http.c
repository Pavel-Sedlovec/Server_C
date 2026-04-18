#include <stdio.h>
#include "http.h"

void parse_req_http(HTTPrequest *req, char *buffer) {
    sscanf(buffer, "%s %s", req->method, req->path);

    char *body_start = strstr(buffer, "\r\n\r\n");
    
    if (body_start) {
        body_start += 4;

        strncpy(req->body, body_start, sizeof(req->body) - 1);
        req->body[sizeof(req->body) - 1] = '\0';
    } else {
        req->body[0] = '\0';
    }
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
