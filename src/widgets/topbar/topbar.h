#ifndef TOPBAR
#define TOPBAR

#include "../../buffer/buffer.h"
#include "../../widget/widget.h"

struct widget *w_topbar(struct widget_dto *wid_dto);

void change_file_name(struct widget *bar, char *filename);
void add_buffer_to_topbar(struct buffer *buff);
void remove_buffer_from_topbar(struct buffer *buff);
void set_topbar_current_buffer(struct buffer *buff);

#endif
