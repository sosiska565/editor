#ifndef WIDGET
#define WIDGET

struct widget {
  int id;
  int x;
  int y;
  int height;
  int width;

  int top;
  int bottom;
  int left;
  int right;
  int center;

  char *content;

  struct widget *parent;
  struct widget *childrens;
  int childrens_counter;

  int fg_color;
  int bg_color;
};

void create_widget(struct widget *wid);
int destroy_widget(struct widget *wid);
void render(struct widget *wid);
void render_all_widgets();

void add_children(struct widget *parent_wid, struct widget *wid);
int remove_children(struct widget *parent_wid, struct widget *wid);

void putchar_in_widget(struct widget *wid, char c, int x, int y);
void putstring_in_widget(struct widget *wid, char *str, int x, int y);
void putstring_in_widgetf(struct widget *wid, int x, int y, char *format, ...);

#endif
