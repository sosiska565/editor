#include "label.h"
#include <string.h>

static void render_label(struct widget *wid) {
  if (wid == NULL)
    return;
  render(wid);
}

static void destroy_label(struct widget *wid) {
  if (wid == NULL)
    return;
  destroy_widget(wid);
}

struct widget *init_label(struct widget_dto *wid_dto) {
  struct widget *wid =
      create_widget(wid_dto->name, wid_dto->x, wid_dto->y, wid_dto->height,
                    wid_dto->width, wid_dto->fg_color, wid_dto->bg_color);

  if (wid == NULL)
    return NULL;

  wid->render = render_label;
  wid->destroy = destroy_label;

  return wid;
}
