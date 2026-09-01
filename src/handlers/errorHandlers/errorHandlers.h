#ifndef ERROR_HANDLERS
#define ERROR_HANDLERS

void errUsage(char *format, ...);
void errExitErrno(char *str);
void errExitErrnoClear(char *str);

#endif
