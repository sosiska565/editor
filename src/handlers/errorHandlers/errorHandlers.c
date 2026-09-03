#include "errorHandlers.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../terminal/terminal.h"

#include "../../debug/debug.h"

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
  write_debug_err(str);
  disable_raw_mode();
  perror(str);
  exit(errno);
}

void errExitErrnoClear(char *str) {
  system("clear");
  errExitErrno(str);
}

void errExitFprintf(char *format, ...) {
  va_list args;
  va_start(args, format);

  disable_raw_mode();
  write_debug_err(format);
  vfprintf(stderr, format, args);
  va_end(args);

  exit(EXIT_FAILURE);
}
