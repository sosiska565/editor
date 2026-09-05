#ifndef BOTTOM_BAR
#define BOTTOM_BAR

#include "../../widget/widget.h"

struct widget *init_bottombar(char *name, int x, int y, int height, int width,
                              int fg_color, int bg_color);

void render_bottombar(struct widget *bar);
void destroy_bottombar(struct widget *bar);

#endif
