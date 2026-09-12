#include "margin.h"
#include "../../debug/debug.h"

static void render_margin(struct widget *wid) {}

struct widget *w_margin(struct widget *wid, struct edges *ed) {
  struct widget *margin = create_contentless_widget();

  margin->render = render_margin;

  int t = ed->all ? ed->all : (ed->vertical ? ed->vertical : ed->top);
  int b = ed->all ? ed->all : (ed->vertical ? ed->vertical : ed->bottom);
  int r = ed->all ? ed->all : (ed->horizontal ? ed->horizontal : ed->right);
  int l = ed->all ? ed->all : (ed->horizontal ? ed->horizontal : ed->left);

  wid->x += l;
  wid->y += t;
  change_size_widget(wid, wid->height - (t + b), wid->width - (l + r));

  return margin;
}
