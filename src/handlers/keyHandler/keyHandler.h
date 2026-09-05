#ifndef KEY_HANDLER
#define KEY_HANDLER

#include <stdio.h>

enum {
  KEY_ARROW_UP = 1000,
  KEY_ARROW_DOWN,
  KEY_ARROW_RIGHT,
  KEY_ARROW_LEFT,
  KEY_DELETE,
  KEY_BACKSPACE,
  KEY_ESCAPE
};

int read_key_nonblock(unsigned char *buff, ssize_t size);
int parse_key(const unsigned char *buff, ssize_t size);
int read_key_and_parse();

#endif
