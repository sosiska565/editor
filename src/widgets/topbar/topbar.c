#include "topbar.h"
#include "../../debug/debug.h"
#include "../label/label.h"
#include <stdlib.h>
#include <string.h>

#define BUFFER_LIST_SEPARATOR " | "
#define CURRENT_BUFFER_MARK "*"

static struct buffer *current_marked_buffer = NULL;

struct widget *init_topbar(char *name, int x, int y, int height, int width,
                           int fg_color, int bg_color) {
  struct widget *bar =
      create_widget(name, x, y, height, width, fg_color, bg_color);
  if (bar == NULL)
    return NULL;

  struct widget *label_buffer_list =
      init_label("label_buffer_list", 0, 0, 1, 1, fg_color, bg_color);

  if (label_buffer_list == NULL) {
    destroy_widget(bar);
    return NULL;
  }

  add_children(bar, label_buffer_list);

  return bar;
}

void change_file_name(struct widget *bar, char *filename) {
  if (bar == NULL || bar->content == NULL)
    return;

  memset(bar->content, ' ', bar->height * bar->width);
  putstring_in_widgetf_aligment(bar, ALIGN_CENTER, "%s", filename);
}

static void refresh_buffer_list_label() {
  struct widget *label_buffer_list =
      find_widget_by_name("_topbar_label_buffer_list");
  if (label_buffer_list == NULL) {
    write_debug_err("label buffer list is null");
    return;
  }

  int total_len = 0;
  for (int i = 0; i < buffers_counter; i++) {
    if (i > 0)
      total_len += strlen(BUFFER_LIST_SEPARATOR);
    if (buffer_list[i] == current_marked_buffer)
      total_len += strlen(CURRENT_BUFFER_MARK);
    total_len += strlen(buffer_list[i]->name);
  }

  if (change_size_widget(label_buffer_list, label_buffer_list->height,
                         total_len > 0 ? total_len : 1) != 0) {
    write_debug_err("failed to resize buffer list label");
    return;
  }

  memset(label_buffer_list->content, ' ',
         (size_t)label_buffer_list->height * label_buffer_list->width);

  int x = 0;
  for (int i = 0; i < buffers_counter; i++) {
    if (i > 0) {
      putstring_in_widget(label_buffer_list, BUFFER_LIST_SEPARATOR, x, 0);
      x += strlen(BUFFER_LIST_SEPARATOR);
    }
    if (buffer_list[i] == current_marked_buffer) {
      putstring_in_widget(label_buffer_list, CURRENT_BUFFER_MARK, x, 0);
      x += strlen(CURRENT_BUFFER_MARK);
    }
    putstring_in_widget(label_buffer_list, buffer_list[i]->name, x, 0);
    x += strlen(buffer_list[i]->name);
  }
}

void set_topbar_current_buffer(struct buffer *buff) {
  current_marked_buffer = buff;
  refresh_buffer_list_label();
}

void add_buffer_to_topbar(struct buffer *buff) {
  add_buffer(buff);
  refresh_buffer_list_label();
}

void remove_buffer_from_topbar(struct buffer *buff) {
  remove_buffer(buff);
  refresh_buffer_list_label();
}

void render_topbar(struct widget *bar) {
  if (bar == NULL)
    return;

  struct widget *label_buffer_list =
      find_widget_by_name("_topbar_label_buffer_list");

  render(bar);
  render_label(label_buffer_list);
}

void destroy_topbar(struct widget *bar) {
  if (bar == NULL)
    return;

  while (bar->childrens_counter > 0) {
    struct widget *child = bar->childrens[0];
    remove_children(bar, child);
    destroy_widget(child);
  }

  destroy_widget(bar);
}
