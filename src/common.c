#include "common.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
xstrdup (const char *s)
{
  size_t len = strlen (s);
  char *ptr;

  ptr = calloc (len + 1, sizeof (char));
  memcpy (ptr, s, len);

  return ptr;
}

_Noreturn void
error (size_t line, const char *fmt, ...)
{
  fprintf (stderr, "fatal-error (line %zu): ", line);

  va_list va;
  va_start (va, fmt);
  vfprintf (stderr, fmt, va);
  va_end (va);

  fprintf (stderr, "\n");
  exit (EXIT_FAILURE);
}

