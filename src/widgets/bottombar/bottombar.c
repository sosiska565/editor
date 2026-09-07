#include "bottombar.h"
#include "../../terminal/terminal.h"
#include "../clock/clock.h"
#include "../label/label.h"
#include <unistd.h>

static void render_bottombar(struct widget *bar) {
  if (bar == NULL)
    return;

  struct widget *clock_wid = find_widget_by_name("_bottombar_clock");
  struct widget *mode_wid = find_widget_by_name("_bottombar_mode");

  render(bar);

  clock_wid->render(clock_wid);
  mode_wid->render(mode_wid);
}

static void destroy_bottombar(struct widget *bar) {
  struct widget *clock_wid = find_widget_by_name("_bottombar_clock");
  struct widget *mode_wid = find_widget_by_name("_bottombar_mode");

  if (clock_wid == NULL || mode_wid == NULL || bar == NULL) {
    return;
  }

  remove_children(bar, clock_wid);
  remove_children(bar, mode_wid);

  destroy_widget(bar);
  clock_wid->destroy(clock_wid);
  mode_wid->destroy(mode_wid);
}

struct widget *init_bottombar(struct widget_dto *wid_dto) {
  struct widget *bar =
      create_widget(wid_dto->name, wid_dto->x, wid_dto->y, wid_dto->height,
                    wid_dto->width, wid_dto->fg_color, wid_dto->bg_color);
  if (bar == NULL)
    return NULL;

  bar->render = render_bottombar;
  bar->destroy = destroy_bottombar;

  struct widget *clock_wid = init_clock(
      &(struct widget_dto){"clock", bar->width - 8, 0, 0, 0,
                           TERMINAL_COLOR_BLACK_FG, TERMINAL_COLOR_WHITE_BG});
  struct widget *mode_wid = init_label(&(struct widget_dto){
      "mode", 0, 0, 1, 20, TERMINAL_COLOR_BLACK_FG, TERMINAL_COLOR_WHITE_BG});

  if (clock_wid == NULL || mode_wid == NULL) {
    destroy_widget(bar);
    return NULL;
  }

  add_children(bar, clock_wid);
  add_children(bar, mode_wid);

  putstring_in_widget(mode_wid, "NORMAL MODE", 0, 0);
  return bar;
}
