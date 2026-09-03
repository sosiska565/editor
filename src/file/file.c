#include "file.h"

#include <fcntl.h>
#include <unistd.h>

#include "../handlers/errorHandlers/errorHandlers.h"

int open_file(char *filename) {
  int editor_fd;
  mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  int fileFlags = O_RDWR | O_CREAT;

  editor_fd = open(filename, fileFlags, filePerms);

  if (editor_fd == -1) {
    errExitErrnoClear("open");
  }

  return editor_fd;
}

void close_file(int fd) {
  if (close(fd) == -1) {
    errExitErrnoClear("close");
  }
}
