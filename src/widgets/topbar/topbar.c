#include "topbar.h"
#include <string.h>

struct widget *init_topbar(char *name, int x, int y, int height, int width,
                           int fg_color, int bg_color) {
  struct widget *bar =
      create_widget(name, x, y, height, width, fg_color, bg_color);
  if (bar == NULL)
    return NULL;

  return bar;
}

void change_file_name(struct widget *bar, char *filename) {
  if (bar == NULL || bar->content == NULL)
    return;

  memset(bar->content, ' ', bar->height * bar->width);
  putstring_in_widgetf_aligment(bar, ALIGN_CENTER, "%s", filename);
}

void render_topbar(struct widget *bar) {
  if (bar == NULL)
    return;

  render(bar);
}

void destroy_topbar(struct widget *bar) {
  if (bar == NULL)
    return;

  while (bar->childrens_counter > 0) {
    struct widget *child = bar->childrens[0];
    remove_children(bar, child);
    destroy_widget(child);
  }

  destroy_widget(bar);
}
