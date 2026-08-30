#include "terminal.h"

#include <asm-generic/ioctls.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>

struct termios orig_termios;
struct terminal term;

void disable_raw_mode() {
  tcsetattr(1, TCSAFLUSH, &orig_termios);
  printf("\033[?25h\033[?1049l");
  fflush(stdout);
}

void enable_raw_mode() {
  tcgetattr(1, &orig_termios);

  atexit(disable_raw_mode);

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);

  tcsetattr(1, TCSAFLUSH, &raw);
}

void init_terminal() {
  printf("\033[?1049h\033[?25l");
  fflush(stdout);

  struct winsize w;
  if (ioctl(1, TIOCGWINSZ, &w) == -1) {
    perror("ioctl");
    exit(EXIT_FAILURE);
  }

  term.height = w.ws_row;
  term.width = w.ws_col;

  term.top = 0;
  term.bottom = w.ws_row;
  term.left = 0;
  term.right = w.ws_col;

  enable_raw_mode();
}

void exit_terminal() {
  disable_raw_mode();
  exit(EXIT_SUCCESS);
}

void move_cursor(int x, int y) { printf("\033[%d;%dH", y + 1, x + 1); }
