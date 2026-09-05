#include "keyHandler.h"
#include "../../handlers/errorHandlers/errorHandlers.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int read_key_nonblock(unsigned char *buff, ssize_t size) {
  int flags = fcntl(0, F_GETFL);
  if (flags == -1) {
    errExitErrnoClear("fcntl");
  }

  fcntl(0, F_SETFL, flags | O_NONBLOCK);
  int numRead;

  if ((numRead = read(0, buff, size)) <= 0) {
    return -1;
  }

  return numRead;
}

int parse_key(const unsigned char *buff, ssize_t size) {
  if (size < 0) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
      errExitErrnoClear("parse key");
    }
    return -1;
  }

  if (size == 0) {
    return -1;
  }

  if (buff[0] == 27) {
    if (size == 1) {
      return KEY_ESCAPE;
    }

    if (size >= 3 && buff[1] == '[') {
      switch (buff[2]) {
      case 'A':
        return KEY_ARROW_UP;
      case 'B':
        return KEY_ARROW_DOWN;
      case 'C':
        return KEY_ARROW_RIGHT;
      case 'D':
        return KEY_ARROW_LEFT;
      case '3':
        if (size >= 4 && buff[3] == '~') {
          return KEY_DELETE;
        }
        break;
      }
    }

    return -1;
  }

  if (size == 1) {
    if (buff[0] == 127 || buff[0] == 8) {
      return KEY_BACKSPACE;
    }

    return buff[0];
  }

  return -1;
}

int read_key_and_parse() {
  unsigned char buff[8];
  int numRead;

  numRead = read_key_nonblock(buff, 8);

  if (numRead == -1)
    return -1;

  return parse_key(buff, numRead);
}
