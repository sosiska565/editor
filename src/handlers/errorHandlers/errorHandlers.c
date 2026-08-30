#include "errorHandlers.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void errUsage(char *format, ...) {
  va_list args;
  va_start(args, format);

  fflush(stdout);

  fprintf(stderr, "Usage: ");
  fprintf(stderr, format, args);

  va_end(args);
  fflush(stdout);

  exit(EXIT_SUCCESS);
}

void errExitErrno(char *str) {
  perror(str);
  exit(errno);
}

void errExitErrnoClear(char *str) {
  system("clear");
  errExitErrno(str);
}
