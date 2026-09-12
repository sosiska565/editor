#ifndef THEME
#define THEME

#include "../terminal/terminal.h"

struct theme {
  unsigned int primary_color;
  unsigned int secondary_color;
  unsigned int background_color;
  unsigned int error_color;
  unsigned int warning_color;

  unsigned int on_primary_color;
  unsigned int on_secondary_color;
  unsigned int on_background_color;
};

extern struct theme app_theme;

#endif
