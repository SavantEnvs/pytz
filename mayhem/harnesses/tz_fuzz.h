/* Declarations of the NetBSD-inspired public API exported by tz/localtime.c
   (enabled by default via NETBSD_INSPIRED in tz/private.h; see tz/newtzset.3). */
#ifndef TZ_FUZZ_H
#define TZ_FUZZ_H
#include <stddef.h>
#include <time.h>
typedef struct state *timezone_t;
timezone_t tzalloc(char const *);
void tzfree(timezone_t);
struct tm *localtime_rz(timezone_t, time_t const *, struct tm *);
time_t mktime_z(timezone_t, struct tm *);
#endif
