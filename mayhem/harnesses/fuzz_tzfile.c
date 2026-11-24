/* TZif binary loader: the input is written to a scratch file and loaded via the
   public tzalloc() API with an absolute path (so TZDIR is not prepended), which runs
   tz/localtime.c tzload/tzloadbody (TZif v1-v4 header, transitions, types, leap
   seconds, POSIX footer -> tzparse), then the conversion APIs. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tz_exercise.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  const char *tmp = getenv("TMPDIR");
  char path[4096];
  snprintf(path, sizeof path, "%s/tzfuzz-XXXXXX", tmp && *tmp ? tmp : "/tmp");
  int fd = mkstemp(path);
  if (fd < 0)
    return 0;
  size_t off = 0;
  while (off < size) {
    ssize_t w = write(fd, data + off, size - off);
    if (w <= 0)
      break;
    off += (size_t)w;
  }
  close(fd);
  timezone_t tz = tzalloc(path);
  unlink(path);
  if (tz) {
    tz_exercise(tz, data, size);
    tzfree(tz);
  }
  return 0;
}
