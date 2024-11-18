#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

char *xstrdup (const char *s);

_Noreturn void error (size_t line, const char *fmt, ...);

#endif // COMMON_H

