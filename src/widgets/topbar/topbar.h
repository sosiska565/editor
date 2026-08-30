#ifndef TOPBAR
#define TOPBAR

#include "../../widget/widget.h"

struct widget *init_topbar(int x, int y, int height, int width, int fg_color,
                           int bg_color);

void change_file_name(char *filename);
void destroy_topbar();
void render_topbar();

#endif
