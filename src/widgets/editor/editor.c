#include "editor.h"

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

void render_editor() { render(&editor_wid); }

void destroy_editor() {
  remove_children(&editor_wid, cursor_wid);

  destroy_widget_debug(&editor_wid);
}
