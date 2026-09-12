#ifndef WIDGET
#define WIDGET

enum widget_type { CONTENT, CONTENT_LESS };

struct widget {
  enum widget_type type;
  char *name;
  int id;
  int x;
  int y;
  int height;
  int width;

  char *content;

  struct widget *parent;
  struct widget **childrens;
  int childrens_counter;

  unsigned int fg_color;
  unsigned int bg_color;

  int (*update)(struct widget *wid, int key);
  void (*render)(struct widget *wid);
  void (*destroy)(struct widget *wid);
};

struct widget_dto {
  char *name;
  int x;
  int y;
  int height;
  int width;
  int fg_color;
  int bg_color;
};

struct edges {
  int top;
  int bottom;
  int left;
  int right;
  int vertical;
  int horizontal;
  int all;
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

struct widget *create_widget(struct widget_dto *wid_dto);
struct widget *create_contentless_widget();
int destroy_widget(struct widget *wid);
void render(struct widget *wid);
void render_widget_tree(struct widget *wid);
void destroy_widget_tree(struct widget *wid);

void add_children(struct widget *parent_wid, struct widget *wid);
int remove_children(struct widget *parent_wid, struct widget *wid);

void putchar_in_widget(struct widget *wid, char c, int x, int y);
void putstring_in_widget(struct widget *wid, char *str, int x, int y);
void putstring_in_widgetf(struct widget *wid, int x, int y, char *format, ...);
void putstring_in_widgetf_aligment(struct widget *wid, int flags, char *format,
                                   ...);

int change_size_widget(struct widget *wid, int height, int width);

struct widget *find_widget_by_name(char *name);

extern struct widget *focused_widget;

#endif
