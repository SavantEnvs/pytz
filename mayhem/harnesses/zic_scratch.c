/* AFL entry point for the zic target: tz/zic.c is compiled with -Dmain=zic_main and linked with this.
 *
 * zic writes compiled TZif files, directories and links under its -d directory. A fixed -d would keep
 * those between inputs (mkdirs/dolink/writezone take different paths when earlier output exists), so
 * every execution gets its own fresh scratch directory under $TMPDIR (fallback /tmp), passed as
 * `-d <dir>` ahead of the Mayhemfile arguments, and removed again when zic exits (zic ends with
 * exit()/return, both of which run atexit handlers). A crash (signal/sanitizer abort) skips the
 * cleanup, but each input still starts in a new, empty directory, so no state carries over.
 */
#define _XOPEN_SOURCE 700
#include <ftw.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int zic_main(int argc, char **argv);

static char scratch[PATH_MAX];

static int rm_entry(const char *path, const struct stat *sb, int type, struct FTW *ftw) {
  (void)sb; (void)type; (void)ftw;
  remove(path);
  return 0;
}

static void cleanup(void) {
  /* zic chdir()s into the -d directory; leave it so the directory itself can be removed. */
  (void)!chdir("/");
  nftw(scratch, rm_entry, 16, FTW_DEPTH | FTW_PHYS);
}

int main(int argc, char **argv) {
  const char *tmp = getenv("TMPDIR");
  if (!tmp || !*tmp) tmp = "/tmp";
  char base[PATH_MAX];
  if (!realpath(tmp, base)) { perror(tmp); return 1; }  /* absolute: zic chdir()s into it */
  if (snprintf(scratch, sizeof scratch, "%s/zic-out.XXXXXX", base) >= (int)sizeof scratch) return 1;
  if (!mkdtemp(scratch)) { perror("mkdtemp"); return 1; }
  atexit(cleanup);

  char **args = calloc((size_t)argc + 3, sizeof *args);
  if (!args) return 1;
  args[0] = argv[0];
  args[1] = "-d";
  args[2] = scratch;
  for (int i = 1; i < argc; i++) args[i + 2] = argv[i];
  return zic_main(argc + 2, args);
}
