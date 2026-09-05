#include "buffer/buffer.h"
#include "debug/debug.h"
#include "handlers/errorHandlers/errorHandlers.h"
#include "handlers/keyHandler/keyHandler.h"
#include "handlers/signalHandlers/signalHandlers.h"
#include "info.h"
#include "terminal/terminal.h"
#include "widget/widget.h"
#include "widgets/display/display.h"

#include <fcntl.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

enum { GEN_CONFIG = 1000 };

int main(int argc, char *argv[]) {
  int opt, fd, flags;
  char *short_options = "hv";
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

  add_buffer(argv[argc - 1]);

  struct widget *display_wid =
      init_display("display", 0, 0, term.height, term.width,
                   TERMINAL_COLOR_WHITE_FG, TERMINAL_DEFAULT_COLOR);

  if (display_wid == NULL)
    errExitFprintf("display_wid is null");

  init_signal_handler();
  write_debug_info("Signal handler init");

  // start main loop
  write_debug_info("Start main loop");

  term.cursor_x = 0;
  term.cursor_y = 1;
  move_cursor_terminal(term.cursor_x, term.cursor_y);

  while (1) {
    term.key = read_key_and_parse();

    clean_cells_buffer();
    render_display(display_wid);
    flush_buffer_to_screen();

    usleep(10000);
  }

  destroy_display(display_wid);

  return EXIT_SUCCESS;
}
