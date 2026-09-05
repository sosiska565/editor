#ifndef DISPLAY
#define DISPLAY

#include "../../widget/widget.h"

struct widget *init_display(char *name, int x, int y, int height, int width,
                            int fg_color, int bg_color);

void render_display(struct widget *disp);
void destroy_display(struct widget *disp);

#endif
