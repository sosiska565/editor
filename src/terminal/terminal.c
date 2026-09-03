#include "terminal.h"

#include <asm-generic/ioctls.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void clean_cells_buffer() {
  for (int i = 0; i < term.width * term.height; i++) {
    term.cells[i].ch = ' ';
    term.cells[i].bg_color = TERMINAL_DEFAULT_COLOR;
    term.cells[i].fg_color = TERMINAL_DEFAULT_COLOR;
  }
}

void init_terminal() {
  printf("\033[?1049h");
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

  term.cells = malloc(term.width * term.height * sizeof(struct cell));
  term.prev_cells = malloc(term.width * term.height * sizeof(struct cell));

  clean_cells_buffer();
  move_cursor_terminal(0, 0);

  memcpy(term.prev_cells, term.cells,
         term.width * term.height * sizeof(struct cell));

  enable_raw_mode();
}

void exit_terminal() {
  disable_raw_mode();
  exit(EXIT_SUCCESS);
}

void move_cursor_terminal(int x, int y) {
  if (x <= 0 || x >= term.width)
    term.cursor_x = 0;
  if (y <= 0 || y >= term.height)
    term.cursor_y = 0;
  printf("\033[%d;%dH", y + 1, x + 1);
  fflush(stdout);
}

void hide_terminal_cursor() { printf("\033[?25l"); }
void show_terminal_cursor() { printf("\033[?25h"); }

void change_color_terminal(int fg, int bg) { printf("\033[%d;%dm", fg, bg); }

void flush_buffet_to_screen() {
  hide_terminal_cursor();

  int last_fg = -1;
  int last_bg = -1;

  for (int i = 0; i < term.height; i++) {
    for (int j = 0; j < term.width; j++) {
      int idx = i * term.width + j;

      struct cell current_cell = term.cells[idx];
      struct cell prev_cell = term.prev_cells[idx];

      if (current_cell.ch == prev_cell.ch &&
          current_cell.bg_color == prev_cell.bg_color &&
          current_cell.fg_color == prev_cell.fg_color) {
        continue;
      }

      move_cursor_terminal(j, i);

      if (current_cell.fg_color != last_fg ||
          current_cell.bg_color != last_bg) {
        change_color_terminal(current_cell.fg_color, current_cell.bg_color);
        last_bg = current_cell.bg_color;
        last_fg = current_cell.fg_color;
      }

      putchar(current_cell.ch);
    }
  }

  memcpy(term.prev_cells, term.cells,
         term.width * term.height * sizeof(struct cell));
  printf("\033[0m");
  show_terminal_cursor();
  move_cursor_terminal(term.cursor_x, term.cursor_y);
  fflush(stdout);
}
