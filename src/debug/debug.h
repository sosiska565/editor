#ifndef DEBUG
#define DEBUG

void init_debug();
int write_debug(const char *str, ...);
int write_debug_ok(const char *str, ...);
int write_debug_info(const char *str, ...);
int write_debug_warn(const char *str, ...);
int write_debug_err(const char *str, ...);
void close_debug();

extern int debug_fd;

#endif
