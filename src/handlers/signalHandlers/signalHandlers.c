#include "signalHandlers.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler_sigint(int sig) {
  system("clear");
  exit(EXIT_SUCCESS);
} // TODO: add safe exit

void init_signal_handler() { signal(SIGINT, handler_sigint); }
