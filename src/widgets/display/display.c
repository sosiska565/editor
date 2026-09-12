#include "display.h"
#include "../../color/color.h"
#include "../../terminal/terminal.h"
#include "../../theme/theme.h"
#include "../bottombar/bottombar.h"
#include "../editor/editor.h"
#include "../margin/margin.h"
#include "../testwidget/testwidget.h"
#include "../topbar/topbar.h"
#include <unistd.h>

static void render_display(struct widget *disp) {
  if (disp == NULL)
    return;

  render(disp);
}

static void destroy_display(struct widget *disp) {
  if (disp == NULL)
    return;

  destroy_widget(disp);
}

struct widget *w_display(struct widget_dto *wid_dto) {
  struct widget *disp = create_widget(wid_dto);
  if (disp == NULL)
    return NULL;

  disp->render = render_display;
  disp->destroy = destroy_display;

  struct widget *bottombar_wid = w_bottombar(&(struct widget_dto){
      "bottombar", disp->x, disp->height - 1, 1, disp->width,
      app_theme.on_primary_color, app_theme.primary_color});
  add_children(disp, bottombar_wid);

  struct widget *topbar_wid = w_topbar(&(struct widget_dto){
      "topbar", disp->x, disp->y, 1, disp->width, app_theme.on_primary_color,
      app_theme.primary_color});
  add_children(disp, topbar_wid);

  struct widget *editor_wid = w_editor(&(struct widget_dto){
      "editor", 0, disp->y + 1, disp->height - 2, disp->width,
      app_theme.on_primary_color, app_theme.background_color});
  add_children(disp, editor_wid);

  term.cursor_x = editor_wid->x;
  term.cursor_y = editor_wid->y;

  if (bottombar_wid == NULL || topbar_wid == NULL || editor_wid == NULL) {
    destroy_widget(disp);
    return NULL;
  }

  return disp;
}
