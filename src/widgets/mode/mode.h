#ifndef MODE
#define MODE

#include "../../widget/widget.h"

struct widget *init_mode(int x, int y, int fg_color, int bg_color);
void render_mode();
void destroy_mode();

#endif
