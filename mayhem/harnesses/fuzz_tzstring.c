/* TZ environment string: the input (NUL-terminated) is passed to the public
   tzalloc() API exactly as a TZ value. A value naming no readable TZif file falls
   through to tz/localtime.c tzparse (getzname/getqzname, getoffset, getrule for
   Jn / n / Mm.w.d, transtime), then the conversion APIs run over the result. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "tz_exercise.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  char *s = malloc(size + 1);
  if (!s)
    return 0;
  memcpy(s, data, size);
  s[size] = '\0';
  timezone_t tz = tzalloc(s);
  if (tz) {
    tz_exercise(tz, data, size);
    tzfree(tz);
  }
  free(s);
  return 0;
}
