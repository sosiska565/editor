#include "editor.h"
#include "../../buffer/buffer.h"
#include "../../debug/debug.h"
#include "../../file/file.h"
#include "../../handlers/errorHandlers/errorHandlers.h"
#include "../../handlers/keyHandler/keyHandler.h"
#include "../../terminal/terminal.h"
#include "../cmdline/cmdline.h"
#include <bits/getopt_core.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { NORMAL_MODE = 1, INSERT_MODE, COMMAND_MODE };

static char *short_options = "wq";

typedef struct {
  char **lines;
  int line_count;
  int row_offset;
  int file_x;
  int file_y;
} EditorState;

static int editor_mode = NORMAL_MODE;
static EditorState E;

void change_mode(int m, struct widget *editor_wid);

static void free_editor_lines() {
  if (E.lines) {
    for (int i = 0; i < E.line_count; i++) {
      free(E.lines[i]);
    }
    free(E.lines);
    E.lines = NULL;
  }
  E.line_count = 0;
}

static void delete_line(int at) {
  if (at < 0 || at >= E.line_count)
    return;

  free(E.lines[at]);
  memmove(&E.lines[at], &E.lines[at + 1],
          sizeof(char *) * (E.line_count - at - 1));
  E.line_count--;
}

static void append_line(const char *s, size_t len) {
  E.lines = realloc(E.lines, sizeof(char *) * (E.line_count + 1));
  E.lines[E.line_count] = malloc(len + 1);
  memcpy(E.lines[E.line_count], s, len);
  E.lines[E.line_count][len] = '\0';
  E.line_count++;
}

static void insert_line(int at, const char *s, size_t len) {
  if (at < 0 || at > E.line_count)
    return;
  E.lines = realloc(E.lines, sizeof(char *) * (E.line_count + 1));
  memmove(&E.lines[at + 1], &E.lines[at], sizeof(char *) * (E.line_count - at));
  E.lines[at] = malloc(len + 1);
  memcpy(E.lines[at], s, len);
  E.lines[at][len] = '\0';
  E.line_count++;
}

static void load_file_to_lines(int fd) {
  free_editor_lines();
  char buf[4096];
  ssize_t nread;
  char *line_buf = NULL;
  size_t line_len = 0;

  while ((nread = read(fd, buf, sizeof(buf))) > 0) {
    for (ssize_t i = 0; i < nread; i++) {
      if (buf[i] == '\r')
        continue;
      if (buf[i] == '\n') {
        append_line(line_buf ? line_buf : "", line_len);
        free(line_buf);
        line_buf = NULL;
        line_len = 0;
      } else {
        line_buf = realloc(line_buf, line_len + 1);
        line_buf[line_len++] = buf[i];
      }
    }
  }
  if (line_buf != NULL || E.line_count == 0) {
    append_line(line_buf ? line_buf : "", line_len);
    free(line_buf);
  }
}

static int save_file(const char *filename) {
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1)
    return -1;

  for (int i = 0; i < E.line_count; i++) {
    write(fd, E.lines[i], strlen(E.lines[i]));
    write(fd, "\n", 1);
  }

  close(fd);
  return 0;
}

static void scroll_editor(struct widget *wid) {
  if (E.file_y < E.row_offset) {
    E.row_offset = E.file_y;
  }
  if (E.file_y >= E.row_offset + wid->height) {
    E.row_offset = E.file_y - wid->height + 1;
  }
  if (E.row_offset < 0)
    E.row_offset = 0;
}

static void refresh_editor_widget(struct widget *wid) {
  memset(wid->content, ' ', wid->width * wid->height);

  for (int y = 0; y < wid->height; y++) {
    int line_idx = E.row_offset + y;
    if (line_idx < E.line_count) {
      char *line = E.lines[line_idx];
      int len = strlen(line);
      for (int x = 0; x < wid->width && x < len; x++) {
        putchar_in_widget(wid, line[x], x, y);
      }
    }
  }
}

struct widget *init_editor(char *name, int x, int y, int height, int width,
                           int fg_color, int bg_color) {
  struct widget *wid =
      create_widget(name, x, y, height, width, fg_color, bg_color);
  if (wid == NULL)
    return NULL;

  char **buflist = get_buffer_list();
  if (buflist == NULL)
    errExitFprintf("buffer list is NULL");

  int fd = open_file(buflist[0]);

  struct widget *topbar_wid = find_widget_by_name("_display_topbar");
  if (topbar_wid == NULL)
    errExitFprintf("topbar widget is NULL");

  putstring_in_widgetf_aligment(topbar_wid, ALIGN_CENTER, buflist[0]);

  load_file_to_lines(fd);
  close(fd);

  E.row_offset = 0;
  E.file_x = 0;
  E.file_y = 0;

  refresh_editor_widget(wid);

  return wid;
}

static void execute_command(const char *cmd) {
  if (cmd == NULL || cmd[0] == '\0')
    return;

  if (cmd[0] == '!') {
    system(cmd + 1);
    return;
  }

  int len = strlen(cmd);
  char *formatted_cmd = malloc(len + 2);
  if (formatted_cmd == NULL)
    return;

  formatted_cmd[0] = '-';
  strcpy(formatted_cmd + 1, cmd);

  char *fake_argv[] = {"editor_cmd", formatted_cmd, NULL};
  int fake_argc = 2;

  optind = 1;
  opterr = 0;

  int opt;
  int flag_w = 0;
  int flag_q = 0;
  int unknown_flag = 0;

  while ((opt = getopt(fake_argc, fake_argv, short_options)) != -1) {
    switch (opt) {
    case 'w':
      flag_w = 1;
      break;
    case 'q':
      flag_q = 1;
      break;
    case '?':
      unknown_flag = 1;
      break;
    }
  }

  free(formatted_cmd);

  if (unknown_flag)
    return;
  if (flag_w) {
    char **buflist = get_buffer_list();
    if (buflist != NULL && buflist[0] != NULL) {
      save_file(buflist[0]); // TODO: make save more files
    }
  }
  if (flag_q) {
    exit_terminal();
  }
}

static void open_cmdline(struct widget *editor_wid) {
  if (editor_wid == NULL)
    return;

  if (find_widget_by_name("_display_editor_cmdline") != NULL)
    return;

  struct widget *cmd_line = init_cmdline(
      "cmdline", (editor_wid->width / 2) - 25, 2, TERMINAL_COLOR_BLACK_FG,
      TERMINAL_COLOR_WHITE_BG, execute_command);

  if (cmd_line == NULL) {
    write_debug_err("editor: failed to open cmdline");
    return;
  }

  add_children(editor_wid, cmd_line);
}

static void exit_command_mode(struct widget *editor_wid) {
  struct widget *cmd_line = find_widget_by_name("_display_editor_cmdline");

  if (cmd_line != NULL && editor_wid != NULL) {
    remove_children(editor_wid, cmd_line);
    destroy_cmdline(cmd_line);
  }

  change_mode(NORMAL_MODE, editor_wid);
}

void change_mode(int m, struct widget *editor_wid) {
  struct widget *mode_wid = find_widget_by_name("_bottombar_mode");
  if (mode_wid == NULL)
    return;

  memset(mode_wid->content, ' ', mode_wid->width * mode_wid->height);

  switch (m) {
  case NORMAL_MODE:
    putstring_in_widget(mode_wid, "NORMAL MODE", 0, 0);
    editor_mode = NORMAL_MODE;
    break;
  case INSERT_MODE:
    putstring_in_widget(mode_wid, "INSERT MODE", 0, 0);
    editor_mode = INSERT_MODE;
    break;
  case COMMAND_MODE:
    putstring_in_widget(mode_wid, "COMMAND MODE", 0, 0);
    editor_mode = COMMAND_MODE;
    open_cmdline(editor_wid);
    break;
  }
}

int get_line_length(int local_y) {
  int line_idx = E.row_offset + local_y;
  if (line_idx < 0 || line_idx >= E.line_count)
    return 0;

  return strlen(E.lines[line_idx]);
}

static void insert_char_at_cursor(char c) {
  if (E.file_y >= E.line_count)
    return;

  char *line = E.lines[E.file_y];
  int len = strlen(line);

  if (E.file_x > len)
    E.file_x = len;

  E.lines[E.file_y] = realloc(line, len + 2);
  line = E.lines[E.file_y];

  memmove(&line[E.file_x + 1], &line[E.file_x], len - E.file_x + 1);
  line[E.file_x] = c;
  E.file_x++;
}

static void insert_newline_at_cursor() {
  if (E.file_y >= E.line_count)
    return;

  char *line = E.lines[E.file_y];
  int len = strlen(line);

  if (E.file_x > len)
    E.file_x = len;

  char *rest = &line[E.file_x];
  insert_line(E.file_y + 1, rest, strlen(rest));

  E.lines[E.file_y][E.file_x] = '\0';
  E.file_y++;
  E.file_x = 0;
}

static void delete_char_at_cursor() {
  if (E.file_y >= E.line_count)
    return;

  if (E.file_x > 0) {
    char *line = E.lines[E.file_y];
    int len = strlen(line);

    memmove(&line[E.file_x - 1], &line[E.file_x], len - E.file_x + 1);
    E.file_x--;
  } else if (E.file_y > 0) {
    int prev_len = strlen(E.lines[E.file_y - 1]);
    int curr_len = strlen(E.lines[E.file_y]);

    E.lines[E.file_y - 1] =
        realloc(E.lines[E.file_y - 1], prev_len + curr_len + 1);
    strcat(E.lines[E.file_y - 1], E.lines[E.file_y]);

    delete_line(E.file_y);
    E.file_y--;
    E.file_x = prev_len;
  }
}

void key_events_handler(struct widget *wid) {
  if (editor_mode == COMMAND_MODE) {
    struct widget *cmd_line = find_widget_by_name("_display_editor_cmdline");
    cmdline_status status = cmdline_process_key(cmd_line, term.key);

    if (status != CMDLINE_ACTIVE) {
      exit_command_mode(wid);
    }
    return;
  }

  if (term.key == KEY_ESCAPE) {
    change_mode(NORMAL_MODE, wid);
  } else if ((term.key == 'i' || term.key == 'a') &&
             editor_mode != INSERT_MODE) {
    change_mode(INSERT_MODE, wid);
    if (term.key == 'a') {
      int len = (E.file_y < E.line_count) ? strlen(E.lines[E.file_y]) : 0;
      if (E.file_x < len) {
        E.file_x++;
      }
    }
  } else if ((term.key == ':') && editor_mode != INSERT_MODE) {
    change_mode(COMMAND_MODE, wid);
  } else if (term.key == KEY_ARROW_LEFT ||
             (term.key == 'h' && editor_mode != INSERT_MODE)) {
    E.file_x--;
  } else if (term.key == KEY_ARROW_DOWN ||
             (term.key == 'j' && editor_mode != INSERT_MODE)) {
    E.file_y++;
  } else if (term.key == KEY_ARROW_RIGHT ||
             (term.key == 'l' && editor_mode != INSERT_MODE)) {
    E.file_x++;
  } else if (term.key == KEY_ARROW_UP ||
             (term.key == 'k' && editor_mode != INSERT_MODE)) {
    E.file_y--;
  } else if (editor_mode == INSERT_MODE) {
    if (term.key == '\n' || term.key == '\r') {
      insert_newline_at_cursor();
    } else if (term.key == KEY_BACKSPACE) {
      delete_char_at_cursor();
    } else if (isprint(term.key)) {
      insert_char_at_cursor(term.key);
    }
  }

  if (E.file_y < 0)
    E.file_y = 0;
  if (E.file_y >= E.line_count)
    E.file_y = E.line_count - 1;
  if (E.file_y < 0)
    E.file_y = 0;

  int current_line_len =
      (E.file_y < E.line_count) ? strlen(E.lines[E.file_y]) : 0;

  if (E.file_x < 0)
    E.file_x = 0;
  if (editor_mode != INSERT_MODE) {
    int max_x = (current_line_len > 0) ? current_line_len - 1 : 0;
    if (E.file_x > max_x)
      E.file_x = max_x;
  } else {
    if (E.file_x > current_line_len)
      E.file_x = current_line_len;
  }

  scroll_editor(wid);
  refresh_editor_widget(wid);

  int render_x = E.file_x;
  int render_y = E.file_y - E.row_offset;

  term.cursor_x = wid->x + render_x;
  term.cursor_y = wid->y + render_y;
  move_cursor_terminal(term.cursor_x, term.cursor_y);
}

void render_editor(struct widget *wid) {
  if (wid == NULL)
    return;
  key_events_handler(wid);
  render(wid);

  struct widget *cmd_line = find_widget_by_name("_display_editor_cmdline");
  if (cmd_line != NULL)
    render_cmdline(cmd_line);
}

void destroy_editor(struct widget *wid) {
  if (wid == NULL)
    return;
  free_editor_lines();
  destroy_widget(wid);
}
