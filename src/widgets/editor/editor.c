#include "editor.h"
#include "../../buffer/buffer.h"
#include "../../file/file.h"
#include "../../handlers/errorHandlers/errorHandlers.h"
#include "../../handlers/keyHandler/keyHandler.h"
#include "../../terminal/terminal.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { NORMAL_MODE = 1, INSERT_MODE, COMMAND_MODE };

static struct widget editor_wid;
static int editor_mode = NORMAL_MODE;
static char *file_buffer;

struct widget *init_editor(char *name, int x, int y, int height, int width,
                           int fg_color, int bg_color) {
  editor_wid = (struct widget){
      .name = "editor",
      .x = x,
      .y = y,
      .height = height,
      .width = width,
      .fg_color = fg_color,
      .bg_color = bg_color,
  };

  if (create_widget(name, &editor_wid) == -1)
    return NULL;

  char **buflist = get_buffer_list();

  if (buflist == NULL)
    errExitFprintf("buffer list is NULL");

  int fd = open_file(buflist[0]);

  struct widget *topbar_wid = find_widget_by_name("topbar");
  if (topbar_wid == NULL)
    errExitFprintf("topbar widget is NULL");

  putstring_in_widgetf_aligment(topbar_wid, ALIGN_CENTER, buflist[0]);

  file_buffer = malloc(width * height);

  int numRead = read(fd, file_buffer, width * height);

  if (numRead == -1)
    errExitErrno("read");

  memcpy(editor_wid.content, file_buffer, width * height);

  return &editor_wid;
}

void change_mode(int m) {
  struct widget *mode_wid = find_widget_by_name("label_mode");
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
    break;
  }
}

void key_events_handler() {
  int local_x = term.cursor_x - editor_wid.x;
  int local_y = term.cursor_y - editor_wid.y;

  if ((local_x - 1) < 0)
    term.cursor_x = editor_wid.x;
  if ((local_x + 1) > editor_wid.width)
    term.cursor_x = editor_wid.width;
  if ((local_y - 1) < 0)
    term.cursor_y = editor_wid.y;
  if ((local_y + 1) > editor_wid.height)
    term.cursor_y = editor_wid.height;

  if (term.key == KEY_ARROW_LEFT ||
      (term.key == 'h' && editor_mode != INSERT_MODE))
    move_cursor_terminal(--term.cursor_x, term.cursor_y);
  if (term.key == KEY_ARROW_DOWN ||
      (term.key == 'j' && editor_mode != INSERT_MODE))
    move_cursor_terminal(term.cursor_x, ++term.cursor_y);
  if (term.key == KEY_ARROW_RIGHT ||
      (term.key == 'l' && editor_mode != INSERT_MODE))
    move_cursor_terminal(++term.cursor_x, term.cursor_y);
  if (term.key == KEY_ARROW_UP ||
      (term.key == 'k' && editor_mode != INSERT_MODE))
    move_cursor_terminal(term.cursor_x, --term.cursor_y);

  if (term.key == KEY_ESCAPE) {
    change_mode(NORMAL_MODE);
    return;
  }
  if ((term.key == 'i' || term.key == 'a') && editor_mode != INSERT_MODE) {
    change_mode(INSERT_MODE);
    return;
  }
  if ((term.key == ':') && editor_mode != INSERT_MODE) {
    change_mode(COMMAND_MODE);
    return;
  }

  if ((term.key == KEY_BACKSPACE) && editor_mode == INSERT_MODE) {
    move_cursor_terminal(--term.cursor_x, term.cursor_y);
    putchar_in_widget(&editor_wid, ' ', local_x - 1, local_y);
  }

  if (isprint(term.key) && editor_mode == INSERT_MODE) {
    putchar_in_widget(&editor_wid, term.key, local_x, local_y);
    move_cursor_terminal(++term.cursor_x, term.cursor_y);
  }
}

void render_editor() {
  key_events_handler();
  render(&editor_wid);
}

void destroy_editor() { destroy_widget(&editor_wid); }
