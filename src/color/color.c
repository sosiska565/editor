#include "color.h"

unsigned int rgb(unsigned int r, unsigned int g, unsigned int b) {
  return (r << 16) | (g << 8) | b;
}
