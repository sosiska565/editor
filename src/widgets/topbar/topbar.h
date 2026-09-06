#ifndef TOPBAR
#define TOPBAR

#include "../../buffer/buffer.h"
#include "../../widget/widget.h"

struct widget *init_topbar(char *name, int x, int y, int height, int width,
                           int fg_color, int bg_color);

void change_file_name(struct widget *bar, char *filename);
void destroy_topbar(struct widget *bar);
void render_topbar(struct widget *bar);
void add_buffer_to_topbar(struct buffer *buff);
void remove_buffer_from_topbar(struct buffer *buff);

#endif
