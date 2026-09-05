#ifndef LABEL
#define LABEL

#include "../../widget/widget.h"

struct widget *init_label(char *name, int x, int y, int fg_color, int bg_color);
void render_label(struct widget *wid);
void destroy_label(struct widget *wid);

#endif
