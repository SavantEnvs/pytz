/* Shared driver: given a timezone_t from tzalloc(), walk the conversion APIs
   (localtime_rz, mktime_z, strftime) over fixed and input-derived instants. */
#ifndef TZ_EXERCISE_H
#define TZ_EXERCISE_H
#include <stdint.h>
#include <string.h>
#include "tz_fuzz.h"

static void tz_exercise(timezone_t tz, const uint8_t *data, size_t size) {
  time_t when[12] = {0, 86400 * 365 * 30, -86400 * 365 * 70, 2147483647,
                     (time_t)-2147483647 - 1, 4102444800, 1700000000,
                     (time_t)1 << 40, -((time_t)1 << 40), 0, 0, 0};
  size_t n = 9;
  for (size_t i = 0; i + 8 <= size && n < 12; i += 8) {
    int64_t v;
    memcpy(&v, data + size - 8 - i, 8);
    when[n++] = (time_t)v;
  }
  for (size_t i = 0; i < n; i++) {
    struct tm tm;
    char buf[256];
    if (!localtime_rz(tz, &when[i], &tm))
      continue;
    strftime(buf, sizeof buf, "%c %Z %z %G-W%V %s", &tm);
    struct tm tm2 = tm;
    (void)mktime_z(tz, &tm2);
    tm2 = tm;
    tm2.tm_isdst = -1;
    tm2.tm_mon += 7;
    (void)mktime_z(tz, &tm2);
  }
}
#endif
