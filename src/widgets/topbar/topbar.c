#include "topbar.h"
#include <string.h>

static struct widget topbar_wid;

struct widget *init_topbar(int x, int y, int height, int width, int fg_color,
                           int bg_color) {
  topbar_wid = (struct widget){.x = x,
                               .y = y,
                               .width = width,
                               .height = height,
                               .fg_color = fg_color,
                               .bg_color = bg_color};

  create_widget_debug(&topbar_wid);

  return &topbar_wid;
}

void change_file_name(char *filename) {
  memset(topbar_wid.content, ' ', topbar_wid.height * topbar_wid.width);
  putstring_in_widgetf_aligment(&topbar_wid, ALIGN_CENTER, "%s", filename);
}

void destroy_topbar() { destroy_widget_debug(&topbar_wid); }

void render_topbar() { render(&topbar_wid); }
