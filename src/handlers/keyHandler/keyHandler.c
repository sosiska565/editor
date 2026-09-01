#include "keyHandler.h"
#include "../../handlers/errorHandlers/errorHandlers.h"
#include <fcntl.h>
#include <unistd.h>

void init_key_handler() {
  int flags = fcntl(0, F_GETFL);
  if (flags == -1) {
    errExitErrnoClear("fcntl");
  }

  fcntl(0, F_SETFL, flags | O_NONBLOCK);
}
