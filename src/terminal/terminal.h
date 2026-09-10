#ifndef TERMINAL
#define TERMINAL

struct cell {
  char ch;
  int fg_color;
  int bg_color;
};

struct terminal {
  int width;
  int height;
  int cursor_x;
  int cursor_y;
  int key;

  struct cell *cells;
  struct cell *prev_cells;

  int top;
  int bottom;
  int left;
  int right;
  int center;
};

void init_terminal();
void move_cursor_terminal(int x, int y);
void exit_terminal();
void hide_terminal_cursor();
void show_terminal_cursor();
void flush_buffer_to_screen();
void change_color_terminal(unsigned int fg, unsigned int bg);
void clean_cells_buffer();
void disable_raw_mode();
void enable_raw_mode();

extern struct terminal term;

#endif
