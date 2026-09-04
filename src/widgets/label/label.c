#include "label.h"
#include <string.h>

static struct widget label_wid;

struct widget *init_label(char *name, int x, int y, int fg_color,
                          int bg_color) {
  label_wid = (struct widget){.x = x,
                              .y = y,
                              .height = 1,
                              .width = 20,
                              .fg_color = fg_color,
                              .bg_color = bg_color};

  if (create_widget(name, &label_wid) == -1)
    return NULL;

  return &label_wid;
}

void render_label() { render(&label_wid); }
void destroy_label() { destroy_widget(&label_wid); }
