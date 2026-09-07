#include "clock.h"
#include <time.h>
#include <unistd.h>

static void render_clock(struct widget *wid) {
  if (wid == NULL)
    return;

  time_t timer;
  struct tm *local_time;
  char time_buffer[9];

  time(&timer);
  local_time = localtime(&timer);

  strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", local_time);

  putstring_in_widgetf_aligment(wid, ALIGN_X_CTR | ALIGN_Y_CTR, "%s",
                                time_buffer);

  render(wid);
}

static void destroy_clock(struct widget *wid) {
  if (wid == NULL)
    return;

  destroy_widget(wid);
}

struct widget *init_clock(struct widget_dto *wid_dto) {
  struct widget *clock_wid =
      create_widget(wid_dto->name, wid_dto->x, wid_dto->y, 1, 8,
                    wid_dto->fg_color, wid_dto->bg_color);

  if (clock_wid == NULL) {
    return NULL;
  }

  clock_wid->render = render_clock;
  clock_wid->destroy = destroy_clock;

  return clock_wid;
}
