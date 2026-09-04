#ifndef WIDGET
#define WIDGET

struct widget {
  char *name;
  int id;
  int x;
  int y;
  int height;
  int width;

  int top;
  int bottom;
  int left;
  int right;
  int center_x;
  int center_y;

  char *content;

  struct widget *parent;
  struct widget *childrens;
  int childrens_counter;

  int fg_color;
  int bg_color;
};

typedef enum {
  ALIGN_LEFT = 1 << 0,
  ALIGN_X_CTR = 1 << 1,
  ALIGN_RIGHT = 1 << 2,

  ALIGN_TOP = 1 << 3,
  ALIGN_Y_CTR = 1 << 4,
  ALIGN_BOTTOM = 1 << 5,

  ALIGN_CENTER = ALIGN_X_CTR | ALIGN_Y_CTR,
  ALIGN_CENTER_LEFT = ALIGN_LEFT | ALIGN_Y_CTR,
  ALIGN_CENTER_RIGHT = ALIGN_RIGHT | ALIGN_Y_CTR
} alignment;

int create_widget(char *name, struct widget *wid);
int destroy_widget(struct widget *wid);
void render(struct widget *wid);
void render_all_widgets();

void add_children(struct widget *parent_wid, struct widget *wid);
int remove_children(struct widget *parent_wid, struct widget *wid);

void putchar_in_widget(struct widget *wid, char c, int x, int y);
void putstring_in_widget(struct widget *wid, char *str, int x, int y);
void putstring_in_widgetf(struct widget *wid, int x, int y, char *format, ...);
void putstring_in_widgetf_aligment(struct widget *wid, int flags, char *format,
                                   ...);

struct widget *find_widget_by_name(char *name);

#endif
