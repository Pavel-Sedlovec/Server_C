#include "signals.h"
#include <signal.h>
#include <errno.h>
#include <stddef.h>

void setup_signals(void (*func)(int)){
    struct sigaction sa = {0};
    sa.sa_handler = func;
    sigaction(SIGINT, &sa, NULL);
}