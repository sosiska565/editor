#include "editor.h"
#include "../../handlers/keyHandler/keyHandler.h"
#include "../../terminal/terminal.h"

static struct widget editor_wid;
static struct widget *cursor_wid;

struct widget *init_editor(int x, int y, int height, int width, int fg_color,
                           int bg_color) {
  editor_wid = (struct widget){
      .x = x,
      .y = y,
      .height = height,
      .width = width,
      .fg_color = fg_color,
      .bg_color = bg_color,
  };

  create_widget_debug(&editor_wid);

  return &editor_wid;
}

void render_editor() {
  if (term.key == KEY_ARROW_LEFT || term.key == 'h')
    move_cursor_terminal(--term.cursor_x, term.cursor_y);
  if (term.key == KEY_ARROW_DOWN || term.key == 'j')
    move_cursor_terminal(term.cursor_x, ++term.cursor_y);
  if (term.key == KEY_ARROW_RIGHT || term.key == 'l')
    move_cursor_terminal(++term.cursor_x, term.cursor_y);
  if (term.key == KEY_ARROW_UP || term.key == 'k')
    move_cursor_terminal(term.cursor_x, --term.cursor_y);

  render(&editor_wid);
}

void destroy_editor() {
  remove_children(&editor_wid, cursor_wid);

  destroy_widget_debug(&editor_wid);
}
