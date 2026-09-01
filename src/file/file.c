#include "file.h"

#include <fcntl.h>
#include <unistd.h>

#include "../handlers/errorHandlers/errorHandlers.h"
#include "../widgets/topbar/topbar.h"

int open_file(char *filename) {
  int fd;
  mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  int fileFlags = O_RDWR | O_CREAT;

  fd = open(filename, fileFlags, filePerms);

  if (fd == -1) {
    errExitErrnoClear("open");
  }

  change_file_name(filename);

  return fd;
}

void close_file(int fd) {
  if (close(fd) == -1) {
    errExitErrnoClear("close");
  }

  change_file_name("");
}
