#include "display.h"
#include "../../terminal/terminal.h"
#include "../bottombar/bottombar.h"
#include "../editor/editor.h"
#include "../topbar/topbar.h"
#include <unistd.h>

struct widget *init_display(char *name, int x, int y, int height, int width,
                            int fg_color, int bg_color) {
  struct widget *disp =
      create_widget(name, x, y, height, width, fg_color, bg_color);
  if (disp == NULL)
    return NULL;

  struct widget *bottombar_wid =
      init_bottombar("bottombar", disp->left, disp->height - 1, 1, disp->width,
                     TERMINAL_COLOR_BLACK_FG, TERMINAL_COLOR_WHITE_BG);
  add_children(disp, bottombar_wid);

  struct widget *topbar_wid =
      init_topbar("topbar", disp->left, disp->top, 1, disp->width,
                  TERMINAL_COLOR_BLACK_FG, TERMINAL_COLOR_WHITE_BG);
  add_children(disp, topbar_wid);

  struct widget *editor_wid =
      init_editor("editor", 0, disp->y + 1, disp->height - 2, disp->width,
                  TERMINAL_COLOR_WHITE_FG, TERMINAL_DEFAULT_COLOR);
  add_children(disp, editor_wid);

  if (bottombar_wid == NULL || topbar_wid == NULL || editor_wid == NULL) {
    destroy_widget(disp);
    return NULL;
  }

  return disp;
}

void render_display(struct widget *disp) {
  if (disp == NULL)
    return;

  struct widget *bottombar_wid = find_widget_by_name("_display_bottombar");
  struct widget *topbar_wid = find_widget_by_name("_display_topbar");
  struct widget *editor_wid = find_widget_by_name("_display_editor");

  render(disp);

  render_bottombar(bottombar_wid);
  render_topbar(topbar_wid);
  render_editor(editor_wid);
}

void destroy_display(struct widget *disp) {
  if (disp == NULL)
    return;

  struct widget *bottombar_wid = find_widget_by_name("_display_bottombar");
  struct widget *topbar_wid = find_widget_by_name("_display_topbar");
  struct widget *editor_wid = find_widget_by_name("_display_editor");

  destroy_bottombar(bottombar_wid);
  destroy_topbar(topbar_wid);
  destroy_editor(editor_wid);

  destroy_widget(disp);
}
