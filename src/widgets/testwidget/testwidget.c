#include "testwidget.h"
#include <stdlib.h>

struct widget *w_testwidget(struct widget_dto *wid_dto) {
  struct widget *wid = create_widget(wid_dto);

  if (wid == NULL)
    return NULL;

  return wid;
}
