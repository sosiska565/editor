#include "bottombar.h"
#include "../../terminal/terminal.h"
#include "../clock/clock.h"
#include "../label/label.h"
#include <unistd.h>

static struct widget bottombar_wid;
static struct widget *clock_wid;
static struct widget *mode_wid;

struct widget *init_bottombar(char *name, int x, int y, int height, int width,
                              int fg_color, int bg_color) {
  bottombar_wid = (struct widget){.name = "bottombar",
                                  .x = x,
                                  .y = y,
                                  .height = height,
                                  .width = width,
                                  .fg_color = fg_color,
                                  .bg_color = bg_color};

  if (create_widget(name, &bottombar_wid) == -1)
    return NULL;

  clock_wid = init_clock("clock", bottombar_wid.width - 8, 0,
                         TERMINAL_COLOR_BLACK_FG, TERMINAL_COLOR_WHITE_BG);

  mode_wid = init_label("label_mode", 0, 0, TERMINAL_COLOR_BLACK_FG,
                        TERMINAL_COLOR_WHITE_BG);

  if (clock_wid == NULL)
    return NULL;
  if (mode_wid == NULL)
    return NULL;

  add_children(&bottombar_wid, clock_wid);
  add_children(&bottombar_wid, mode_wid);

  putstring_in_widget(mode_wid, "NORMAL MODE", 0, 0);

  return &bottombar_wid;
}

void render_bottombar() {
  render(&bottombar_wid);
  render_clock();
  render_label();
}

void destroy_bottombar() {
  remove_children(&bottombar_wid, clock_wid);
  remove_children(&bottombar_wid, mode_wid);

  destroy_widget(&bottombar_wid);
  destroy_clock();
  destroy_label();
}
