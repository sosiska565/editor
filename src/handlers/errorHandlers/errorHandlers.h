#ifndef ERROR_HANDLERS
#define ERROR_HANDLERS

#include <stdio.h>

void errUsage(char *format, ...);
void errExitErrno(char *str);
void errExitErrnoClear(char *str);

#endif
