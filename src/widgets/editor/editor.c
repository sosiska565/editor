#include "editor.h"
#include "../../buffer/buffer.h"
#include "../../debug/debug.h"
#include "../../file/file.h"
#include "../../handlers/keyHandler/keyHandler.h"
#include "../../terminal/terminal.h"
#include "../cmdline/cmdline.h"
#include "../topbar/topbar.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { NORMAL_MODE = 1, INSERT_MODE, COMMAND_MODE };

struct buffer *current_buffer = NULL;
static struct widget *g_editor_wid = NULL;

#define E (*current_buffer)

static int editor_mode = NORMAL_MODE;

void change_mode(int m, struct widget *editor_wid);

static void render_editor(struct widget *wid);
static void destroy_editor(struct widget *wid);

static void delete_line(int at) {
  if (at < 0 || at >= E.line_count)
    return;

  free(E.lines[at]);
  memmove(&E.lines[at], &E.lines[at + 1],
          sizeof(char *) * (E.line_count - at - 1));
  E.line_count--;
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

static void load_buffer_content(struct buffer *buf) {
  if (buf == NULL || buf->lines != NULL)
    return;

  if (buf->fd < 0) {
    buf->lines = malloc(sizeof(char *));
    buf->lines[0] = strdup("");
    buf->line_count = 1;
    return;
  }

  char rbuf[4096];
  ssize_t nread;
  char *line_buf = NULL;
  size_t line_len = 0;
  char **lines = NULL;
  int count = 0;

  while ((nread = read(buf->fd, rbuf, sizeof(rbuf))) > 0) {
    for (ssize_t i = 0; i < nread; i++) {
      if (rbuf[i] == '\r')
        continue;
      if (rbuf[i] == '\n') {
        lines = realloc(lines, sizeof(char *) * (count + 1));
        lines[count] = malloc(line_len + 1);
        memcpy(lines[count], line_buf ? line_buf : "", line_len);
        lines[count][line_len] = '\0';
        count++;
        free(line_buf);
        line_buf = NULL;
        line_len = 0;
      } else {
        line_buf = realloc(line_buf, line_len + 1);
        line_buf[line_len++] = rbuf[i];
      }
    }
  }
  if (line_buf != NULL || count == 0) {
    lines = realloc(lines, sizeof(char *) * (count + 1));
    lines[count] = malloc(line_len + 1);
    memcpy(lines[count], line_buf ? line_buf : "", line_len);
    lines[count][line_len] = '\0';
    count++;
    free(line_buf);
  }

  buf->lines = lines;
  buf->line_count = count;

  close(buf->fd);
  buf->fd = -1;
}

static void save_buffer_to_disk(struct buffer *buf) {
  if (buf == NULL || buf->lines == NULL)
    return;

  int fd = open(buf->name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1)
    return;

  for (int i = 0; i < buf->line_count; i++) {
    write(fd, buf->lines[i], strlen(buf->lines[i]));
    write(fd, "\n", 1);
  }

  close(fd);
}

static void save_current_buffer(void) {
  if (current_buffer != NULL) {
    save_buffer_to_disk(current_buffer);
  }
}

static void save_all_buffers(void) {
  for (int i = 0; i < buffers_counter; i++) {
    save_buffer_to_disk(buffer_list[i]);
  }
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

struct widget *init_editor(struct widget_dto *wid_dto) {
  struct widget *wid =
      create_widget(wid_dto->name, wid_dto->x, wid_dto->y, wid_dto->height,
                    wid_dto->width, wid_dto->fg_color, wid_dto->bg_color);
  if (wid == NULL)
    return NULL;

  wid->render = render_editor;
  wid->destroy = destroy_editor;

  current_buffer = buffer_list[0];
  g_editor_wid = wid;

  struct widget *topbar_wid = find_widget_by_name("_display_topbar");
  if (topbar_wid != NULL) {
    for (int i = 0; i < buffers_counter; i++) {
      write_debug_info("Name: %s, fd: %d", buffer_list[i]->name,
                       buffer_list[i]->fd);
      add_buffer_to_topbar(buffer_list[i]);
    }
    set_topbar_current_buffer(current_buffer);
  }

  load_buffer_content(current_buffer);

  refresh_editor_widget(wid);

  return wid;
}

static void switch_to_buffer(struct buffer *buf, struct widget *wid) {
  if (buf == NULL || buf == current_buffer)
    return;

  load_buffer_content(buf);
  current_buffer = buf;
  set_topbar_current_buffer(current_buffer);

  if (wid != NULL) {
    scroll_editor(wid);
    refresh_editor_widget(wid);
  }
}

static void switch_to_adjacent_buffer(int direction, struct widget *wid) {
  if (buffers_counter <= 1)
    return;

  int idx = 0;
  for (int i = 0; i < buffers_counter; i++) {
    if (buffer_list[i] == current_buffer) {
      idx = i;
      break;
    }
  }

  idx = (idx + direction + buffers_counter) % buffers_counter;
  switch_to_buffer(buffer_list[idx], wid);
}

static void open_and_switch(const char *filename) {
  struct buffer *filebuf = open_file((char *)filename);

  if (filebuf == NULL) {
    filebuf = find_buffer_by_name((char *)filename);
  } else {
    write_debug_info("Open file name: %s, fd: %d", filebuf->name, filebuf->fd);
    add_buffer_to_topbar(filebuf);
  }

  switch_to_buffer(filebuf, g_editor_wid);
}

static void close_buffer_command(const char *name) {
  struct buffer *target =
      (name != NULL) ? find_buffer_by_name((char *)name) : current_buffer;

  if (target == NULL || buffers_counter <= 1)
    return;

  int idx = -1;
  for (int i = 0; i < buffers_counter; i++) {
    if (buffer_list[i] == target) {
      idx = i;
      break;
    }
  }

  if (idx < 0)
    return;

  int was_current = (target == current_buffer);
  struct buffer *fallback = (idx == 0) ? buffer_list[1] : buffer_list[idx - 1];

  if (was_current) {
    current_buffer = fallback;
    load_buffer_content(current_buffer);
    set_topbar_current_buffer(current_buffer);
  }

  write_debug_info("fd: %d", target->fd);

  if (target->fd >= 0)
    close_file(target);
  remove_buffer_from_topbar(target);

  if (was_current && g_editor_wid != NULL) {
    scroll_editor(g_editor_wid);
    refresh_editor_widget(g_editor_wid);
  }
}

static int is_flag_cluster(const char *s) {
  if (s == NULL || s[0] == '\0')
    return 0;

  for (int i = 0; s[i] != '\0'; i++) {
    if (s[i] != 'w' && s[i] != 'q' && s[i] != 'a') {
      return 0;
    }
  }

  return 1;
}

static void execute_command(const char *cmd) {
  if (cmd == NULL || cmd[0] == '\0')
    return;

  if (cmd[0] == '!') {
    system(cmd + 1);
    return;
  }

  char *cmd_copy = strdup(cmd);
  if (cmd_copy == NULL)
    return;

  char *tokens[16];
  int token_count = 0;

  char *token = strtok(cmd_copy, " ");
  while (token != NULL && token_count < 16) {
    tokens[token_count++] = token;
    token = strtok(NULL, " ");
  }

  if (token_count == 0) {
    free(cmd_copy);
    return;
  }

  char *name = tokens[0];
  char *arg = (token_count > 1) ? tokens[1] : NULL;
  int quit = 0;

  if (strcmp(name, "o") == 0 || strcmp(name, "open") == 0) {
    if (arg != NULL) {
      open_and_switch(arg);
    }
  } else if (strcmp(name, "c") == 0 || strcmp(name, "close") == 0) {
    close_buffer_command(arg);
  } else if (is_flag_cluster(name)) {
    int has_w = strchr(name, 'w') != NULL;
    int has_q = strchr(name, 'q') != NULL;
    int has_a = strchr(name, 'a') != NULL;

    if (has_w) {
      if (has_a) {
        save_all_buffers();
      } else {
        save_current_buffer();
      }
    }
    if (has_q) {
      quit = 1;
    }
  }

  free(cmd_copy);

  if (quit) {
    exit_terminal();
  }
}

static void open_cmdline(struct widget *editor_wid) {
  if (editor_wid == NULL)
    return;

  if (find_widget_by_name("_display_editor_cmdline") != NULL)
    return;

  struct widget *cmd_line = init_cmdline(
      &(struct widget_dto){"cmdline", (editor_wid->width / 2) - 25, 2, 0, 0,
                           TERMINAL_COLOR_BLACK_FG, TERMINAL_COLOR_WHITE_BG},
      execute_command);

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
    cmd_line->destroy(cmd_line);
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

  if (term.key == 'H' && editor_mode != INSERT_MODE) {
    switch_to_adjacent_buffer(-1, wid);
  }
  if (term.key == 'L' && editor_mode != INSERT_MODE) {
    switch_to_adjacent_buffer(1, wid);
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

static void render_editor(struct widget *wid) {
  if (wid == NULL)
    return;
  key_events_handler(wid);
  render(wid);

  struct widget *cmd_line = find_widget_by_name("_display_editor_cmdline");
  if (cmd_line != NULL)
    cmd_line->render(cmd_line);
}

static void destroy_editor(struct widget *wid) {
  if (wid == NULL)
    return;
  for (int i = 0; i < buffers_counter; i++) {
    free_buffer_lines(buffer_list[i]);
  }
  destroy_widget(wid);
}
