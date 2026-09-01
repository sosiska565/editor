#include "widget.h"
#include "../debug/debug.h"
#include "../terminal/terminal.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct widget *widgets = NULL;
int widgets_counter = 0;
int global_id = 0;

void add_widget(struct widget *wid) {
  widgets_counter++;

  struct widget *new_array = (struct widget *)realloc(
      widgets, widgets_counter * sizeof(struct widget));

  if (new_array == NULL) {
    abort();
  }

  widgets = new_array;
  widgets[widgets_counter - 1] = *wid;
}

int remove_widget_from_id(int id) {
  int target_index = -1;

  for (int i = 0; i < widgets_counter; i++) {
    if (widgets[i].id == id) {
      target_index = i;
      break;
    }
  }

  if (target_index == -1) {
    return -1;
  }

  if (widgets[target_index].content != NULL) {
    free(widgets[target_index].content);
  }

  for (int i = target_index; i < widgets_counter - 1; i++) {
    widgets[i] = widgets[i + 1];
  }

  widgets_counter--;

  if (widgets_counter > 0) {
    struct widget *new_array = (struct widget *)realloc(
        widgets, widgets_counter * sizeof(struct widget));
    if (new_array == NULL) {
      abort();
    }
    widgets = new_array;
  } else {
    free(widgets);
    widgets = NULL;
  }

  return id;
}

void create_widget(struct widget *wid) {
  if (wid == NULL) {
    fprintf(stderr, "widget is null");
    exit(EXIT_FAILURE);
  }

  char *con = (char *)malloc(wid->height * wid->width * sizeof(char));
  memset(con, ' ', wid->height * wid->width);

  if (con == NULL) {
    abort();
  }

  wid->id = global_id++;
  wid->childrens_counter = 0;
  wid->childrens = NULL;
  wid->parent = NULL;
  wid->content = con;

  wid->top = wid->y;
  wid->bottom = wid->y + wid->height;
  wid->left = wid->x;
  wid->right = wid->x + wid->width;
  wid->center_x = wid->width / 2;
  wid->center_y = wid->height / 2;

  add_widget(wid);
}

int destroy_widget(struct widget *wid) {
  if (wid == NULL) {
    fprintf(stderr, "ERROR: widget is null");
    exit(EXIT_FAILURE);
  }
  return remove_widget_from_id(wid->id);
}

void add_children(struct widget *parent_wid, struct widget *wid) {
  if (parent_wid == NULL) {
    fprintf(stderr, "ERROR: parent widget is null");
    exit(EXIT_FAILURE);
  }
  if (wid == NULL) {
    fprintf(stderr, "ERROR: widget is null");
    exit(EXIT_FAILURE);
  }

  parent_wid->childrens_counter++;

  struct widget *new_array = (struct widget *)realloc(
      parent_wid->childrens,
      parent_wid->childrens_counter * sizeof(struct widget));

  if (new_array == NULL) {
    perror("realloc");
    exit(EXIT_FAILURE);
  }

  parent_wid->childrens = new_array;

  wid->parent = parent_wid;
  wid->x = parent_wid->x + wid->x;
  wid->y = parent_wid->y + wid->y;

  parent_wid->childrens[parent_wid->childrens_counter - 1] = (*wid);
}

int remove_children(struct widget *parent_wid, struct widget *wid) {
  if (parent_wid == NULL) {
    fprintf(stderr, "ERROR: parent widget is null");
    exit(EXIT_FAILURE);
  }
  if (wid == NULL) {
    fprintf(stderr, "ERROR: widget is null");
    exit(EXIT_FAILURE);
  }

  parent_wid->childrens_counter--;

  if (parent_wid->childrens_counter == 0) {
    free(parent_wid->childrens);
    return -1;
  }

  int target_index = -1;

  for (int i = 0; i < parent_wid->childrens_counter; i++) {
    if (parent_wid->childrens[i].id == wid->id) {
      target_index = i;
      break;
    }
  }

  if (target_index == -1) {
    return -1;
  }

  for (int i = target_index; i < parent_wid->childrens_counter - 1; i++) {
    parent_wid->childrens[i] = parent_wid->childrens[i + 1];
  }

  return wid->id;
}

void render(struct widget *wid) {
  for (int i = 0; i < wid->height; i++) {
    for (int j = 0; j < wid->width; j++) {
      int screen_x = wid->x + j;
      int screen_y = wid->y + i;

      if (screen_x >= 0 && screen_x < term.width && screen_y >= 0 &&
          screen_y < term.height) {

        int buf_idx = screen_y * term.width + screen_x;
        int wid_idx = i * wid->width + j;

        term.cells[buf_idx].ch = wid->content[wid_idx];
        term.cells[buf_idx].fg_color = wid->fg_color;
        term.cells[buf_idx].bg_color = wid->bg_color;
      }
    }
  }

  hide_terminal_cursor();
  move_cursor_terminal(term.cursor_x, term.cursor_y);
  show_terminal_cursor();
}

void convert_local_coordinates_to_global(int *x, int *y, int wid_x, int wid_y) {
  (*x) = (*x) + wid_x;
  (*y) = (*y) + wid_y;
}

void putchar_in_widget(struct widget *wid, char c, int x, int y) {
  wid->content[y * wid->width + x] = c;
}

void putstring_in_widget(struct widget *wid, char *str, int x, int y) {
  for (int i = 0; str[i] != '\0'; i++) {
    putchar_in_widget(wid, str[i], x++, y);
  }
}

void render_all_widgets() {
  for (int i = 0; i < widgets_counter; i++) {
    render(&widgets[i]);
  }
}

void putstring_in_widgetf(struct widget *wid, int x, int y, char *format, ...) {
  va_list args;
  va_list args_copy;
  char *buffer;

  va_start(args, format);

  va_copy(args_copy, args);
  int str_len = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  if (str_len < 0) {
    write_debug_err("widget.c: str_len < 0");
  }

  buffer = (char *)malloc(str_len + 1);
  if (!buffer)
    exit(EXIT_FAILURE);

  vsnprintf(buffer, str_len + 1, format, args);

  putstring_in_widget(wid, buffer, x, y);

  free(buffer);
}

void vputstring_in_widgetf(struct widget *wid, int x, int y, char *format,
                           va_list args) {
  va_list args_copy;
  char *buffer;

  va_copy(args_copy, args);
  int str_len = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  if (str_len < 0) {
    write_debug_err("widget.c: str_len < 0");
  }

  buffer = (char *)malloc(str_len + 1);
  if (!buffer)
    exit(EXIT_FAILURE);

  vsnprintf(buffer, str_len + 1, format, args);

  putstring_in_widget(wid, buffer, x, y);

  free(buffer);
}

void create_widget_debug(struct widget *wid) {
  create_widget(wid);
  write_debug_info("widget %d created", wid->id);
}

int destroy_widget_debug(struct widget *wid) {
  int id = destroy_widget(wid);

  if (id < 0) {
    write_debug_err("widget destroy err. Id: %d", id);
    exit(EXIT_FAILURE);
  }

  write_debug_info("widget %d destroy", id);

  return id;
}

void get_aligment_coordinates(int *x, int *y, int width, int height,
                              int content_width, int content_height,
                              int flags) {
  if (flags & ALIGN_X_CTR) {
    (*x) = (width - content_width) / 2;
  } else if (flags & ALIGN_RIGHT) {
    (*x) = width - content_width;
  } else {
    (*x) = 0;
  }

  if (flags & ALIGN_Y_CTR) {
    (*y) = (height - content_height) / 2;
  } else if (flags & ALIGN_BOTTOM) {
    (*y) = height - content_height;
  } else {
    (*y) = 0;
  }

  if ((*x) < 0)
    (*x) = 0;
  if ((*y) < 0)
    (*y) = 0;
}

void putstring_in_widgetf_aligment(struct widget *wid, int flags, char *format,
                                   ...) {
  int target_x;
  int target_y;
  int str_len;
  va_list args;
  va_list args_copy;

  va_start(args, format);

  va_copy(args_copy, args);
  str_len = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  get_aligment_coordinates(&target_x, &target_y, wid->width, wid->height,
                           str_len, 0, flags);

  vputstring_in_widgetf(wid, target_x, target_y, format, args);

  va_end(args);
}
