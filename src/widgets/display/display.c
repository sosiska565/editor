#include "display.h"

#include "../../terminal/terminal.h"
#include "../bottombar/bottombar.h"
#include "../clock/clock.h"

#include <stdio.h>
#include <stdlib.h>

static struct widget display_wid;
static struct widget *clock_wid;
static struct widget *bottombar_wid;

struct widget *init_display(int x, int y, int height, int width, int fg_color,
                            int bg_color) {
  display_wid = (struct widget){.x = x,
                                .y = y,
                                .height = height,
                                .width = width,
                                .fg_color = fg_color,
                                .bg_color = bg_color};

  create_widget(&display_wid);

  bottombar_wid =
      init_bottombar(0, term.height - 1, 1, term.width, TERMINAL_COLOR_BLACK_FG,
                     TERMINAL_COLOR_WHITE_BG);

  clock_wid = init_clock(bottombar_wid->right - 8, 0, TERMINAL_COLOR_BLACK_FG,
                         TERMINAL_COLOR_WHITE_BG);

  add_children(&display_wid, bottombar_wid);
  add_children(bottombar_wid, clock_wid);

  return &display_wid;
}

void render_display() {
  render(&display_wid);
  render_bottombar();
  render_clock();
}
void destroy_display() {
  if (destroy_widget(&display_wid) < 0) {
    fprintf(stderr, "ERROR: failed destroy widget");
    exit(EXIT_FAILURE);
  }
}
