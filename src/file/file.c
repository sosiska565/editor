#include "file.h"

#include "../buffer/buffer.h"
#include <fcntl.h>
#include <unistd.h>

#include "../handlers/errorHandlers/errorHandlers.h"

struct buffer *open_file(char *filename) {
  int editor_fd;
  mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  int fileFlags = O_RDWR | O_CREAT;

  editor_fd = open(filename, fileFlags, filePerms);

  if (editor_fd == -1) {
    errExitErrnoClear("open");
  }

  return add_buffer(&(struct buffer){filename, editor_fd});
}

void close_file(struct buffer *buff) {
  if (buff == NULL)
    return;

  if (close(buff->fd) == -1) {
    errExitErrnoClear("close");
  }

  buff->fd = -1;
}
