#include "bottombar.h"
#include "../../terminal/terminal.h"
#include "../clock/clock.h"
#include "../label/label.h"
#include <unistd.h>

struct widget *init_bottombar(char *name, int x, int y, int height, int width,
                              int fg_color, int bg_color) {
  struct widget *bar =
      create_widget(name, x, y, height, width, fg_color, bg_color);
  if (bar == NULL)
    return NULL;

  struct widget *clock_wid =
      init_clock("clock", bar->width - 8, 0, TERMINAL_COLOR_BLACK_FG,
                 TERMINAL_COLOR_WHITE_BG);
  struct widget *mode_wid = init_label("mode", 0, 0, TERMINAL_COLOR_BLACK_FG,
                                       TERMINAL_COLOR_WHITE_BG);

  if (clock_wid == NULL || mode_wid == NULL) {
    destroy_widget(bar);
    return NULL;
  }

  add_children(bar, clock_wid);
  add_children(bar, mode_wid);

  putstring_in_widget(mode_wid, "NORMAL MODE", 0, 0);
  return bar;
}

void render_bottombar(struct widget *bar) {
  if (bar == NULL)
    return;

  struct widget *clock_wid = find_widget_by_name("_bottombar_clock");
  struct widget *mode_wid = find_widget_by_name("_bottombar_mode");

  render(bar);

  render_clock(clock_wid);
  render_label(mode_wid);
}

void destroy_bottombar(struct widget *bar) {
  struct widget *clock_wid = find_widget_by_name("_bottombar_clock");
  struct widget *mode_wid = find_widget_by_name("_bottombar_mode");

  if (clock_wid == NULL || mode_wid == NULL || bar == NULL) {
    return;
  }

  destroy_widget(bar);
  destroy_clock(clock_wid);
  destroy_label(mode_wid);
}
