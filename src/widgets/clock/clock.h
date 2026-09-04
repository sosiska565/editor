#ifndef CLOCK
#define CLOCK

#include "../../widget/widget.h"

struct widget *init_clock(char *name, int x, int y, int fg_color, int bg_color);
void render_clock();
void destroy_clock();

#endif
