#include "display.h"

#include "../../terminal/terminal.h"
#include "../bottombar/bottombar.h"
#include "../clock/clock.h"
#include "../editor/editor.h"
#include "../topbar/topbar.h"

static struct widget display_wid;
static struct widget *clock_wid;
static struct widget *bottombar_wid;
static struct widget *topbar_wid;
static struct widget *editor_wid;
static int key;

struct widget *init_display(int x, int y, int height, int width, int fg_color,
                            int bg_color) {
  display_wid = (struct widget){.x = x,
                                .y = y,
                                .height = height,
                                .width = width,
                                .fg_color = fg_color,
                                .bg_color = bg_color};

  create_widget_debug(&display_wid);

  // init bottombar and clock
  bottombar_wid = init_bottombar(display_wid.left, display_wid.height - 1, 1,
                                 display_wid.width, TERMINAL_COLOR_BLACK_FG,
                                 TERMINAL_COLOR_WHITE_BG);

  clock_wid = init_clock(bottombar_wid->right - 8, 0, TERMINAL_COLOR_BLACK_FG,
                         TERMINAL_COLOR_WHITE_BG);

  // init topbar
  topbar_wid =
      init_topbar(display_wid.left, display_wid.top, 1, display_wid.width,
                  TERMINAL_COLOR_BLACK_FG, TERMINAL_COLOR_WHITE_BG);

  editor_wid = init_editor(0, display_wid.y + 1, display_wid.height - 2,
                           display_wid.width, TERMINAL_COLOR_WHITE_FG,
                           TERMINAL_COLOR_BLACK_BG);

  add_children(&display_wid, bottombar_wid);
  add_children(&display_wid, topbar_wid);
  add_children(bottombar_wid, clock_wid);
  add_children(&display_wid, editor_wid);

  return &display_wid;
}

void render_display() {
  render(&display_wid);
  render_bottombar();
  render_clock();
  render_topbar();
  render_editor();
}

void destroy_display() {
  remove_children(&display_wid, bottombar_wid);
  remove_children(bottombar_wid, clock_wid);
  remove_children(&display_wid, topbar_wid);
  remove_children(&display_wid, editor_wid);

  destroy_display();
  destroy_bottombar();
  destroy_topbar();
  destroy_editor();
  destroy_clock();
}
