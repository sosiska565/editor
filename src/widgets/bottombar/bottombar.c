#include "bottombar.h"
#include "../../color/color.h"
#include "../../terminal/terminal.h"
#include "../../theme/theme.h"
#include "../clock/clock.h"
#include "../label/label.h"
#include <unistd.h>

struct penis {
  int age;
};

static void render_bottombar(struct widget *bar) {
  if (bar == NULL)
    return;

  render(bar);
}

static void destroy_bottombar(struct widget *bar) {
  if (bar == NULL)
    return;
  destroy_widget(bar);
}

struct widget *w_bottombar(struct widget_dto *wid_dto) {
  struct widget *bar = create_widget(wid_dto);
  if (bar == NULL)
    return NULL;

  bar->render = render_bottombar;
  bar->destroy = destroy_bottombar;

  struct widget *clock_wid = w_clock(&(struct widget_dto){
      "clock", bar->width - 8, 0, 1, 8, app_theme.on_secondary_color,
      app_theme.secondary_color});
  struct widget *mode_wid = w_label(
      &(struct widget_dto){"mode", 0, 0, 1, 20, app_theme.on_secondary_color,
                           app_theme.secondary_color});

  if (clock_wid == NULL || mode_wid == NULL) {
    destroy_widget(bar);
    return NULL;
  }

  add_children(bar, clock_wid);
  add_children(bar, mode_wid);

  putstring_in_widget(mode_wid, "NORMAL MODE", 0, 0);
  return bar;
}
