#include "debug/debug.h"
#include "handlers/signalHandlers/signalHandlers.h"
#include "info.h"
#include "terminal/terminal.h"
#include "widget/widget.h"
#include "widgets/display/display.h"
#include <asm-generic/ioctls.h>

#include <fcntl.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

enum { GEN_CONFIG = 1000 };

int main(int argc, char *argv[]) {
  int opt;
  char *short_options = "hv";
  int fd;
  struct option long_options[] = {{"help", no_argument, NULL, 'h'},
                                  {"gen-config", no_argument, NULL, GEN_CONFIG},
                                  {"version", no_argument, NULL, 'v'},
                                  {NULL, 0, NULL, 0}};

  init_debug();
  write_debug_info("Debug init");

  if (argc < 2) {
    print_help(argv[0]);
    exit(EXIT_FAILURE);
  }

  while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (opt) {
    case 'h':
      print_help(argv[0]);
      exit(EXIT_SUCCESS);
    case 'v':
      print_version();
      exit(EXIT_SUCCESS);
    case GEN_CONFIG:
      // TODO: make gen-config
      exit(EXIT_SUCCESS);
    default:
      exit(EXIT_FAILURE);
    }
  }

  init_terminal();

  write_debug_info("Terminal init %dx%d", term.width, term.height);

  struct widget *display_wid =
      init_display(0, 0, term.height, term.width, TERMINAL_COLOR_WHITE_FG,
                   TERMINAL_COLOR_BLACK_BG);

  init_signal_handler();

  write_debug_info("Signal handler init");
  write_debug_info("Start main loop");

  while (1) {
    render_display();
    sleep(1);
  }

  destroy_display();

  return EXIT_SUCCESS;
}
