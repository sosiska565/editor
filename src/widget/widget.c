#include "widget.h"
#include "../debug/debug.h"
#include "../terminal/terminal.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct widget **widgets = NULL;

int widgets_counter = 0;
int global_id = 0;

void add_widget(struct widget *wid) {
  widgets_counter++;

  struct widget **new_array = (struct widget **)realloc(
      widgets, widgets_counter * sizeof(struct widget *));

  if (new_array == NULL) {
    abort();
  }

  widgets = new_array;
  widgets[widgets_counter - 1] = wid;
}

int remove_widget_from_id(int id) {
  int target_index = -1;

  for (int i = 0; i < widgets_counter; i++) {
    if (widgets[i]->id == id) {
      target_index = i;
      break;
    }
  }

  if (target_index == -1) {
    return -1;
  }

  if (widgets[target_index]->content != NULL) {
    free(widgets[target_index]->content);
  }

  if (widgets[target_index]->name != NULL) {
    free(widgets[target_index]->name);
  }

  free(widgets[target_index]);

  for (int i = target_index; i < widgets_counter - 1; i++) {
    widgets[i] = widgets[i + 1];
  }

  widgets_counter--;

  if (widgets_counter > 0) {
    struct widget **new_array = (struct widget **)realloc(
        widgets, widgets_counter * sizeof(struct widget *));
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

int exists_widget_by_name(char *name) {
  for (int i = 0; i < widgets_counter; i++) {
    if (strcmp(widgets[i]->name, name) == 0) {
      return 1;
    }
  }

  return 0;
}

int exists_widget_by_id(int id) {
  for (int i = 0; i < widgets_counter; i++) {
    if (widgets[i]->id == id) {
      return 1;
    }
  }

  return 0;
}

struct widget *find_widget_by_name(char *name) {
  for (int i = 0; i < widgets_counter; i++) {
    if (strcmp(widgets[i]->name, name) == 0) {
      return widgets[i];
    }
  }

  return NULL;
}

struct widget *create_widget(char *name, int x, int y, int height, int width,
                             int fg_color, int bg_color) {
  if (exists_widget_by_name(name) == 1) {
    return NULL;
  }

  if (exists_widget_by_id(global_id + 1) == 1) {
    return NULL;
  }

  struct widget *wid = (struct widget *)malloc(sizeof(struct widget));
  if (wid == NULL) {
    return NULL;
  }

  char *con = (char *)malloc(height * width * sizeof(char));
  if (con == NULL) {
    return NULL;
  }
  memset(con, ' ', height * width);

  wid->id = global_id++;
  wid->name = strdup(name);
  if (wid->name == NULL) {
    free(con);
    free(wid);
    return NULL;
  }

  wid->x = x;
  wid->y = y;
  wid->height = height;
  wid->width = width;
  wid->fg_color = fg_color;
  wid->bg_color = bg_color;

  wid->childrens_counter = 0;
  wid->childrens = NULL;
  wid->parent = NULL;
  wid->content = con;

  wid->top = y;
  wid->bottom = y + height;
  wid->left = x;
  wid->right = x + width;
  wid->center_x = width / 2;
  wid->center_y = height / 2;

  add_widget(wid);

  return wid;
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

  struct widget **new_array = (struct widget **)realloc(
      parent_wid->childrens,
      parent_wid->childrens_counter * sizeof(struct widget *));

  if (new_array == NULL) {
    perror("realloc");
    exit(EXIT_FAILURE);
  }

  parent_wid->childrens = new_array;

  wid->parent = parent_wid;
  wid->x = parent_wid->x + wid->x;
  wid->y = parent_wid->y + wid->y;

  int wid_name_len = strlen(parent_wid->name) + strlen(wid->name) + 3;
  char *wid_name = (char *)malloc(wid_name_len);

  char *parent_name_start =
      (parent_wid->name[0] == '_') ? (parent_wid->name + 1) : parent_wid->name;
  snprintf(wid_name, wid_name_len, "_%s_%s", parent_name_start, wid->name);

  free(wid->name);
  wid->name = wid_name;

  parent_wid->childrens[parent_wid->childrens_counter - 1] = wid;
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

  int target_index = -1;

  for (int i = 0; i < parent_wid->childrens_counter; i++) {
    if (parent_wid->childrens[i]->id == wid->id) {
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

  parent_wid->childrens_counter--;

  if (parent_wid->childrens_counter == 0) {
    free(parent_wid->childrens);
    parent_wid->childrens = NULL;
  } else {
    struct widget **new_array = (struct widget **)realloc(
        parent_wid->childrens,
        parent_wid->childrens_counter * sizeof(struct widget *));
    if (new_array != NULL) {
      parent_wid->childrens = new_array;
    }
  }

  wid->parent = NULL;

  return wid->id;
}

void render(struct widget *wid) {
  if (wid == NULL || wid->content == NULL)
    return;

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
}

void convert_local_coordinates_to_global(int *x, int *y, int wid_x, int wid_y) {
  (*x) = (*x) + wid_x;
  (*y) = (*y) + wid_y;
}

void putchar_in_widget(struct widget *wid, char c, int x, int y) {
  if (wid == NULL || wid->content == NULL)
    return;
  if (x >= 0 && x < wid->width && y >= 0 && y < wid->height) {
    wid->content[y * wid->width + x] = c;
  }
}

void putstring_in_widget(struct widget *wid, char *str, int x, int y) {
  if (wid == NULL || str == NULL)
    return;
  for (int i = 0; str[i] != '\0'; i++) {
    putchar_in_widget(wid, str[i], x++, y);
  }
}

void render_all_widgets() {
  for (int i = 0; i < widgets_counter; i++) {
    render(widgets[i]);
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
    va_end(args);
    return;
  }

  buffer = (char *)malloc(str_len + 1);
  if (!buffer)
    exit(EXIT_FAILURE);

  vsnprintf(buffer, str_len + 1, format, args);

  putstring_in_widget(wid, buffer, x, y);

  free(buffer);
  va_end(args);
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
    return;
  }

  buffer = (char *)malloc(str_len + 1);
  if (!buffer)
    exit(EXIT_FAILURE);

  vsnprintf(buffer, str_len + 1, format, args);

  putstring_in_widget(wid, buffer, x, y);

  free(buffer);
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
