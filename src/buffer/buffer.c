#include "buffer.h"
#include "../handlers/errorHandlers/errorHandlers.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct buffer **buffer_list;
int buffers_counter = 0;

struct buffer *add_buffer(struct buffer *buff) {
  if (buff == NULL)
    return NULL;

  if (exists_buffer(buff) == 1) {
    return NULL;
  }

  int new_counter = buffers_counter + 1;
  struct buffer **new_buffer_list = (struct buffer **)realloc(
      buffer_list, new_counter * sizeof(struct buffer *));

  if (new_buffer_list == NULL) {
    errExitFprintf("no memory");
    return NULL;
  }
  buffer_list = new_buffer_list;
  buffers_counter = new_counter;

  buffer_list[buffers_counter - 1] =
      (struct buffer *)malloc(sizeof(struct buffer));
  if (buffer_list[buffers_counter - 1] == NULL) {
    errExitFprintf("no memory for buffer struct");
    return NULL;
  }

  buffer_list[buffers_counter - 1]->name = strdup(buff->name);
  buffer_list[buffers_counter - 1]->fd = buff->fd;

  return buffer_list[buffers_counter - 1];
}

int remove_buffer(struct buffer *buff) {
  if (buff == NULL || buff == NULL)
    return 0;

  for (int i = 0; i < buffers_counter; i++) {
    if (strcmp(buffer_list[i]->name, buff->name) == 0) {

      free(buffer_list[i]->name);
      close(buffer_list[i]->fd);
      free(buffer_list[i]);

      for (int j = i; j < buffers_counter - 1; j++) {
        buffer_list[j] = buffer_list[j + 1];
      }

      buffers_counter--;

      if (buffers_counter == 0) {
        free(buffer_list);
        buffer_list = NULL;
      } else {
        struct buffer **new_buffer_list = (struct buffer **)realloc(
            buffer_list, buffers_counter * sizeof(struct buffer *));
        if (new_buffer_list != NULL) {
          buffer_list = new_buffer_list;
        }
      }

      return 1;
    }
  }

  return 0;
}

int exists_buffer(struct buffer *buff) {
  if (buff == NULL || buff == NULL)
    return 0;

  for (int i = 0; i < buffers_counter; i++) {
    if (strcmp(buffer_list[i]->name, buff->name) == 0) {
      return 1;
    }
  }

  return 0;
}

struct buffer *find_buffer_by_name(char *name) {
  for (int i = 0; i < buffers_counter; i++) {
    if (strcmp(buffer_list[i]->name, name) == 0) {
      return buffer_list[i];
    }
  }
  return NULL;
}
