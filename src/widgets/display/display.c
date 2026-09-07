#include "display.h"
#include "../../terminal/terminal.h"
#include "../bottombar/bottombar.h"
#include "../editor/editor.h"
#include "../testwidget/testwidget.h"
#include "../topbar/topbar.h"
#include <unistd.h>

static void render_display(struct widget *disp) {
  if (disp == NULL)
    return;

  struct widget *bottombar_wid = find_widget_by_name("_display_bottombar");
  struct widget *topbar_wid = find_widget_by_name("_display_topbar");
  struct widget *editor_wid = find_widget_by_name("_display_editor");

  render(disp);

  bottombar_wid->render(bottombar_wid);
  bottombar_wid->render(bottombar_wid);
  topbar_wid->render(topbar_wid);
  editor_wid->render(editor_wid);
}

static void destroy_display(struct widget *disp) {
  if (disp == NULL)
    return;

  struct widget *bottombar_wid = find_widget_by_name("_display_bottombar");
  struct widget *topbar_wid = find_widget_by_name("_display_topbar");
  struct widget *editor_wid = find_widget_by_name("_display_editor");

  bottombar_wid->destroy(bottombar_wid);
  topbar_wid->destroy(topbar_wid);
  editor_wid->destroy(editor_wid);

  destroy_widget(disp);
}

struct widget *init_display(struct widget_dto *wid_dto) {
  struct widget *disp =
      create_widget(wid_dto->name, wid_dto->x, wid_dto->y, wid_dto->height,
                    wid_dto->width, wid_dto->fg_color, wid_dto->bg_color);
  if (disp == NULL)
    return NULL;

  disp->render = render_display;
  disp->destroy = destroy_display;

  struct widget *bottombar_wid = init_bottombar(&(struct widget_dto){
      "bottombar", disp->left, disp->height - 1, 1, disp->width,
      TERMINAL_COLOR_BLACK_FG, TERMINAL_COLOR_WHITE_BG});
  add_children(disp, bottombar_wid);

  struct widget *topbar_wid = init_topbar(
      &(struct widget_dto){"topbar", disp->left, disp->top, 1, disp->width,
                           TERMINAL_COLOR_BLACK_FG, TERMINAL_COLOR_WHITE_BG});
  add_children(disp, topbar_wid);

  struct widget *editor_wid = init_editor(&(struct widget_dto){
      "editor", 0, disp->y + 1, disp->height - 2, disp->width,
      TERMINAL_COLOR_WHITE_FG, TERMINAL_DEFAULT_COLOR});
  add_children(disp, editor_wid);

  struct widget *test_wid = init_testwidget(
      &(struct widget_dto){"Penis", 10, 10, 20, 20, TERMINAL_COLOR_WHITE_FG,
                           TERMINAL_COLOR_BLACK_BG});

  if (bottombar_wid == NULL || topbar_wid == NULL || editor_wid == NULL) {
    destroy_widget(disp);
    return NULL;
  }

  return disp;
}
