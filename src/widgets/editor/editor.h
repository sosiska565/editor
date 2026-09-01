#ifndef EDITOR
#define EDITOR

#include "../../widget/widget.h"

struct widget *init_editor(int x, int y, int height, int width, int fg_color,
                           int bg_color);
void render_editor();
void destroy_editor();

#endif
