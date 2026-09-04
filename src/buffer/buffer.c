#include "buffer.h"
#include "../handlers/errorHandlers/errorHandlers.h"
#include <stdlib.h>
#include <string.h>

static char **buffer_list;
static int buffers_counter = 0;

void add_buffer(char *buf) {
  buffers_counter++;

  char **new_buffer_list =
      (char **)realloc(buffer_list, buffers_counter * sizeof(char *));

  if (new_buffer_list == NULL) {
    errExitFprintf("no memory");
  }
  buffer_list = new_buffer_list;

  buffer_list[buffers_counter - 1] = strdup(buf);
}

int remove_buffer(char *buf) {
  for (int i = 0; i < buffers_counter; i++) {
    if (strcmp(buffer_list[i], buf) == 0) {

      free(buffer_list[i]);

      for (int j = i; j < buffers_counter - 1; j++) {
        buffer_list[j] = buffer_list[j + 1];
      }

      buffers_counter--;

      if (buffers_counter == 0) {
        free(buffer_list);
        buffer_list = NULL;
      } else {
        char **new_buffer_list =
            (char **)realloc(buffer_list, buffers_counter * sizeof(char *));
        if (new_buffer_list != NULL) {
          buffer_list = new_buffer_list;
        }
      }

      return 1;
    }
  }

  return 0;
}

int exists_buffer(char *buf) {
  for (int i = 0; i < buffers_counter; i++) {
    if (strcmp(buffer_list[i], buf) == 0) {
      return 1;
    }
  }

  return 0;
}

char **get_buffer_list() { return buffer_list; }
