#ifndef CMDLINE
#define CMDLINE

#include "../../widget/widget.h"

typedef enum {
  CMDLINE_ACTIVE,
  CMDLINE_SUBMITTED,
  CMDLINE_CANCELLED
} cmdline_status;

typedef void (*cmdline_submit_cb)(const char *cmd);

struct widget *init_cmdline(struct widget_dto *wid_dto,
                            cmdline_submit_cb on_submit);

cmdline_status cmdline_process_key(struct widget *wid, int key);

#endif
