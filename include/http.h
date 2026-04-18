#ifndef HTTP.H
#define HTTP.H

typedef struct{
    char method[16];
    char path[128];
    char body[2048];
}HTTPrequest;

void parse_req_http(HTTPrequest *req, char *buffer);
void send_response(int conn, int status, const char *type, const char *body);

#endif