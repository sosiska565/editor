#include "cmdline.h"
#include "../../handlers/keyHandler/keyHandler.h"
#include "../../terminal/terminal.h"
#include <ctype.h>
#include <string.h>

#define CMDLINE_PROMPT "cmd> "
#define CMDLINE_PROMPT_X 2
#define CMDLINE_PROMPT_Y 1
#define CMDLINE_MAX_LEN 44

static char cmd_buf[CMDLINE_MAX_LEN + 1];
static int cmd_len = 0;

static int saved_cursor_x = -1;
static int saved_cursor_y = -1;

static cmdline_submit_cb submit_cb = NULL;

static void redraw_cmdline(struct widget *wid) {
  memset(wid->content, ' ', wid->width * wid->height);
  putstring_in_widget(wid, CMDLINE_PROMPT, CMDLINE_PROMPT_X, CMDLINE_PROMPT_Y);
  putstring_in_widget(wid, cmd_buf,
                      CMDLINE_PROMPT_X + (int)strlen(CMDLINE_PROMPT),
                      CMDLINE_PROMPT_Y);

  term.cursor_x =
      wid->x + CMDLINE_PROMPT_X + (int)strlen(CMDLINE_PROMPT) + cmd_len;
  term.cursor_y = wid->y + CMDLINE_PROMPT_Y;
  move_cursor_terminal(term.cursor_x, term.cursor_y);
}

struct widget *init_cmdline(char *name, int x, int y, int fg_color,
                            int bg_color, cmdline_submit_cb on_submit) {
  struct widget *wid = create_widget(name, x, y, 3, 50, fg_color, bg_color);
  if (wid == NULL)
    return NULL;

  cmd_len = 0;
  cmd_buf[0] = '\0';
  submit_cb = on_submit;

  saved_cursor_x = term.cursor_x;
  saved_cursor_y = term.cursor_y;

  redraw_cmdline(wid);

  return wid;
}

cmdline_status cmdline_process_key(struct widget *wid, int key) {
  if (wid == NULL || key == -1)
    return CMDLINE_ACTIVE;

  if (key == KEY_ESCAPE) {
    cmd_len = 0;
    cmd_buf[0] = '\0';
    return CMDLINE_CANCELLED;
  }

  if (key == '\n' || key == '\r') {
    cmd_buf[cmd_len] = '\0';
    if (submit_cb != NULL)
      submit_cb(cmd_buf);
    cmd_len = 0;
    cmd_buf[0] = '\0';
    return CMDLINE_SUBMITTED;
  }

  if (key == KEY_BACKSPACE) {
    if (cmd_len > 0) {
      cmd_len--;
      cmd_buf[cmd_len] = '\0';
    }
    redraw_cmdline(wid);
    return CMDLINE_ACTIVE;
  }

  if (key >= 0 && key < 256 && isprint(key) && cmd_len < CMDLINE_MAX_LEN) {
    cmd_buf[cmd_len++] = (char)key;
    cmd_buf[cmd_len] = '\0';
    redraw_cmdline(wid);
  }

  return CMDLINE_ACTIVE;
}

void render_cmdline(struct widget *wid) {
  if (wid == NULL)
    return;
  render(wid);
}

void destroy_cmdline(struct widget *wid) {
  if (wid == NULL)
    return;

  cmd_len = 0;
  cmd_buf[0] = '\0';
  submit_cb = NULL;

  if (saved_cursor_x != -1 && saved_cursor_y != -1) {
    term.cursor_x = saved_cursor_x;
    term.cursor_y = saved_cursor_y;
    move_cursor_terminal(term.cursor_x, term.cursor_y);

    saved_cursor_x = -1;
    saved_cursor_y = -1;
  }

  destroy_widget(wid);
}
