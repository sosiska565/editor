#include "signalHandlers.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler_sigint(int sig) {}

void handler_sigwinch(int sig) {}
void init_signal_handler() { signal(SIGINT, handler_sigint); }
