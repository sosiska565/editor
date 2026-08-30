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
      move_cursor(j + wid->x, i + wid->y);
      printf("\033[%d;%dm", wid->fg_color, wid->bg_color);
      putchar(wid->content[i * wid->width + j]);
    }
  }

  printf("\033[0m");
  fflush(stdout);
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
