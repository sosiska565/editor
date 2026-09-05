#include "label.h"
#include <string.h>

struct widget *init_label(char *name, int x, int y, int fg_color,
                          int bg_color) {
  struct widget *wid = create_widget(name, x, y, 1, 20, fg_color, bg_color);

  if (wid == NULL)
    return NULL;

  return wid;
}

void render_label(struct widget *wid) {
  if (wid == NULL)
    return;
  render(wid);
}

void destroy_label(struct widget *wid) {
  if (wid == NULL)
    return;
  destroy_widget(wid);
}
