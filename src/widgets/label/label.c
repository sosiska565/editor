#include "label.h"
#include <string.h>

struct widget *init_label(struct widget_dto *wid_dto) {
  struct widget *wid =
      create_widget(wid_dto->name, wid_dto->x, wid_dto->y, wid_dto->height,
                    wid_dto->width, wid_dto->fg_color, wid_dto->bg_color);

  if (wid == NULL)
    return NULL;

  return wid;
}
