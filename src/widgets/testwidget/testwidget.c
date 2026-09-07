#include "testwidget.h"
#include <stdlib.h>

static void destroy_clock(struct widget *wid) { destroy_widget(wid); }

static void render_clock(struct widget *wid) { render(wid); }

struct widget *init_testwidget(struct widget_dto *wid_dto) {
  struct widget *wid =
      create_widget(wid_dto->name, wid_dto->x, wid_dto->y, wid_dto->height,
                    wid_dto->width, wid_dto->fg_color, wid_dto->bg_color);

  if (wid == NULL)
    return NULL;

  wid->render = render_clock;
  wid->destroy = destroy_clock;

  putstring_in_widgetf_aligment(wid, ALIGN_CENTER, "Penis");

  return wid;
}
