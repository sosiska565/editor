#ifndef BOTTOM_BAR
#define BOTTOM_BAR

#include "../../widget/widget.h"

struct widget *init_bottombar(int x, int y, int height, int width, int fg_color,
                              int bg_color);

void render_bottombar();
void destroy_bottombar();

#endif
