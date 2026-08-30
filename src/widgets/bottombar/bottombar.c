#include "bottombar.h"
#include <stdio.h>
#include <stdlib.h>

static struct widget bottombar_wid;

struct widget *init_bottombar(int x, int y, int height, int width, int fg_color,
                              int bg_color) {
  bottombar_wid = (struct widget){.x = x,
                                  .y = y,
                                  .height = height,
                                  .width = width,
                                  .fg_color = fg_color,
                                  .bg_color = bg_color};

  create_widget(&bottombar_wid);

  return &bottombar_wid;
}

void render_bottombar() { render(&bottombar_wid); }

void destroy_bottombar() {
  if (destroy_widget(&bottombar_wid) < 0) {
    fprintf(stderr, "ERROR: failed destroy widget");
    exit(EXIT_FAILURE);
  }
}
