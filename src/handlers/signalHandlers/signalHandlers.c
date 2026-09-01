#include "signalHandlers.h"

#include "../../terminal/terminal.h"
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler_sigint(int sig) {
  system("clear");
  exit(EXIT_SUCCESS);
} // TODO: add safe exit

void handler_sigwinch(int sig) {}
void init_signal_handler() { signal(SIGINT, handler_sigint); }
