#include "info.h"
#include <stdio.h>
#include <unistd.h>

void print_help(char *argv1) {
  fprintf(stderr, "Usage:\n\t%s [options] ... FILE", argv1);
  fprintf(stderr, "\nOptions:");
  fprintf(stderr, "\n\t-h, --help\tPrint this help message");
  fprintf(stderr, "\n\t-v, --version\tPrint version and info");
  fprintf(stderr, "\n\t--gen-config\tGenerate config file");
}

void print_version() {
  printf("%s v%s\nAuthor: %s\nLicense: %s", NAME, VERSION, AUTHOR, LICENSE);
}
