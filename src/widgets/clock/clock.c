#include "clock.h"
#include <time.h>
#include <unistd.h>

struct widget *init_clock(char *name, int x, int y, int fg_color,
                          int bg_color) {
  struct widget *clock_wid =
      create_widget(name, x, y, 1, 8, fg_color, bg_color);

  if (clock_wid == NULL) {
    return NULL;
  }

  return clock_wid;
}

void render_clock(struct widget *wid) {
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

void destroy_clock(struct widget *wid) {
  if (wid == NULL)
    return;

  destroy_widget(wid);
}
