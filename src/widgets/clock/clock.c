#include "clock.h"
#include <time.h>
#include <unistd.h>

static struct widget clock_wid;

struct widget *init_clock(int x, int y, int fg_color, int bg_color) {
  clock_wid = (struct widget){
      .x = x,
      .y = y,
      .height = 1,
      .width = 8,
      .fg_color = fg_color,
      .bg_color = bg_color,
  };

  create_widget_debug(&clock_wid);

  return &clock_wid;
}

void render_clock() {
  time_t timer;
  struct tm *local_time;
  char time_buffer[9];

  time(&timer);

  local_time = localtime(&timer);

  strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", local_time);

  putstring_in_widgetf_aligment(&clock_wid, ALIGN_X_CTR | ALIGN_Y_CTR, "%s",
                                time_buffer);

  render(&clock_wid);
}
void destroy_clock() { destroy_widget_debug(&clock_wid); }
