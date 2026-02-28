#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

extern int count_message;
extern pthread_mutex_t mutex;

void* start_client(void* arg);
#endif