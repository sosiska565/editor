#include "debug/debug.h"
#include "handlers/errorHandlers/errorHandlers.h"
#include "handlers/keyHandler/keyHandler.h"
#include "handlers/signalHandlers/signalHandlers.h"
#include "info.h"
#include "terminal/terminal.h"
#include "widget/widget.h"
#include "widgets/display/display.h"

#include "color/color.h"
#include "file/file.h"
#include <fcntl.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "color/color.h"
#include "theme/theme.h"

enum { GEN_CONFIG = 1000 };

int main(int argc, char *argv[]) {
  int opt, fd, flags;
  char *short_options = "hv";
  struct option long_options[] = {{"help", no_argument, NULL, 'h'},
                                  {"gen-config", no_argument, NULL, GEN_CONFIG},
                                  {"version", no_argument, NULL, 'v'},
                                  {NULL, 0, NULL, 0}};

  app_theme = (struct theme){.primary_color = rgb(99, 102, 241),
                             .secondary_color = rgb(16, 185, 129),
                             .background_color = rgb(30, 41, 59),
                             .error_color = rgb(239, 68, 68),
                             .warning_color = rgb(245, 158, 11),
                             .on_primary_color = rgb(255, 255, 255),
                             .on_secondary_color = rgb(15, 23, 42),
                             .on_background_color = rgb(241, 245, 249)};

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

  open_file(argv[argc - 1]);

  struct widget *display_wid =
      w_display(&(struct widget_dto){"display", 0, 0, term.height, term.width,
                                     rgb(255, 255, 255), rgb(0, 0, 0)});

  if (display_wid == NULL)
    errExitFprintf("display_wid is null");

  init_signal_handler();
  write_debug_info("Signal handler init");

  // start main loop
  write_debug_info("Start main loop");

  while (1) {
    term.key = read_key_and_parse();

    if (term.key != -1 && focused_widget != NULL &&
        focused_widget->update != NULL) {
      focused_widget->update(focused_widget, term.key);
    }
    clean_cells_buffer();
    render_widget_tree(display_wid);
    flush_buffer_to_screen();

    usleep(10000);
  }

  destroy_widget_tree(display_wid);

  return EXIT_SUCCESS;
}
