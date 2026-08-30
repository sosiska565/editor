#ifndef TERMINAL
#define TERMINAL

struct terminal {
  int width;
  int height;

  int top;
  int bottom;
  int left;
  int right;
  int center;
};

#define TERMINAL_COLOR_BLACK_FG 30
#define TERMINAL_COLOR_BLACK_BG 40
#define TERMINAL_COLOR_RED_FG 31
#define TERMINAL_COLOR_RED_BG 41
#define TERMINAL_COLOR_GREEN_FG 32
#define TERMINAL_COLOR_GREEN_BG 42
#define TERMINAL_COLOR_YELLOW_FG 33
#define TERMINAL_COLOR_YELLOW_BG 43
#define TERMINAL_COLOR_BLUE_FG 34
#define TERMINAL_COLOR_BLUE_BG 44
#define TERMINAL_COLOR_PURPLE_FG 35
#define TERMINAL_COLOR_PURPLE_BG 45
#define TERMINAL_COLOR_CYAN_FG 36
#define TERMINAL_COLOR_CYAN_BG 46
#define TERMINAL_COLOR_WHITE_FG 37
#define TERMINAL_COLOR_WHITE_BG 47
#define TERMINAL_DEFAULT_COLOR 0

void init_terminal();
void move_cursor(int x, int y);
void exit_terminal();

extern struct terminal term;

#endif
