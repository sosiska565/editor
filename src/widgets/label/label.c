#include "label.h"
#include <string.h>

struct widget *w_label(struct widget_dto *wid_dto) {
  struct widget *wid = create_widget(wid_dto);

  if (wid == NULL)
    return NULL;

  return wid;
}
