#include "debug.h"

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../handlers/errorHandlers/errorHandlers.h"

int debug_fd;

void init_debug() {
  mode_t file_flags = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  debug_fd = open("debug_file.log", O_CREAT | O_RDWR | O_APPEND, file_flags);

  if (debug_fd == -1) {
    errExitErrnoClear("open");
  }
}

static int vwrite_debug(const char *status, const char *str, va_list args) {
  va_list args_copy;
  char *buffer;
  int numWrite;
  char prefix[64];

  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  if (status && status[0] != '\0') {
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%m/%d/%Y - %H:%M:%S", timeinfo);
    snprintf(prefix, sizeof(prefix), "[ %s %s ] ", time_str, status);
  } else {
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%m/%d/%Y - %H:%M:%S", timeinfo);
    snprintf(prefix, sizeof(prefix), "[ %s ] ", time_str);
  }

  va_copy(args_copy, args);
  int str_len = vsnprintf(NULL, 0, str, args_copy);
  va_end(args_copy);

  if (str_len < 0) {
    return -1;
  }

  int prefix_len = strlen(prefix);
  int total_len = prefix_len + str_len;

  buffer = (char *)malloc(total_len + 1);
  if (!buffer)
    return -1;

  strcpy(buffer, prefix);
  vsnprintf(buffer + prefix_len, str_len + 1, str, args);

  numWrite = write(debug_fd, buffer, total_len);

  if (numWrite == -1) {
    free(buffer);
    errExitErrnoClear("write");
  }

  free(buffer);
  return numWrite;
}

int write_debug(const char *str, ...) {
  va_list args;
  va_start(args, str);
  int result = vwrite_debug("", str, args);
  va_end(args);
  return result;
}

int write_debug_ok(const char *str, ...) {
  va_list args;
  va_start(args, str);
  int result = vwrite_debug("OK", str, args);
  va_end(args);
  return result;
}

int write_debug_warn(const char *str, ...) {
  va_list args;
  va_start(args, str);
  int result = vwrite_debug("WARN", str, args);
  va_end(args);
  return result;
}

int write_debug_err(const char *str, ...) {
  va_list args;
  va_start(args, str);
  int result = vwrite_debug("ERROR", str, args);
  va_end(args);
  return result;
}

int write_debug_info(const char *str, ...) {
  va_list args;
  va_start(args, str);
  int result = vwrite_debug("INFO", str, args);
  va_end(args);
  return result;
}

void close_debug() {
  if (close(debug_fd) == -1) {
    errExitErrnoClear("close");
  }
}
