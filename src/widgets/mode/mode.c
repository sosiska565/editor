#include "mode.h"
#include <string.h>

static struct widget mode_wid;

struct widget *init_mode(int x, int y, int fg_color, int bg_color) {
  mode_wid = (struct widget){.name = "mode",
                             .x = x,
                             .y = y,
                             .height = 1,
                             .width = 20,
                             .fg_color = fg_color,
                             .bg_color = bg_color};

  if (create_widget(&mode_wid) == -1)
    return NULL;

  putstring_in_widget(&mode_wid, "NORMAL MODE", 0, 0);

  return &mode_wid;
}

void render_mode() { render(&mode_wid); }
void destroy_mode() { destroy_widget_debug(&mode_wid); }
