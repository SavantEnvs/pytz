/* strftime format strings (tz/strftime.c _fmt: every conversion, E/O modifiers,
   ISO week %G/%V, %s via mktime, %Z/%z). The first 8 bytes are a time_t that
   localtime_r() (tz/localtime.c) turns into a struct tm -- strftime's contract
   requires a normalized struct tm, as a real caller passes -- and the rest is the
   NUL-terminated format. TZ is fixed once at startup and never changed by an input. */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int LLVMFuzzerInitialize(int *argc, char ***argv) {
  (void)argc; (void)argv;
  setenv("TZ", "EST5EDT,M3.2.0,M11.1.0", 1);
  tzset();
  return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  int64_t v;
  struct tm tm;
  if (size < sizeof v)
    return -1;
  memcpy(&v, data, sizeof v);
  time_t t = (time_t)v;
  if (!localtime_r(&t, &tm))
    return 0;
  size_t flen = size - sizeof v;
  char *fmt = malloc(flen + 1);
  if (!fmt)
    return 0;
  memcpy(fmt, data + sizeof v, flen);
  fmt[flen] = '\0';
  char buf[512];
  strftime(buf, sizeof buf, fmt, &tm);
  free(fmt);
  return 0;
}
