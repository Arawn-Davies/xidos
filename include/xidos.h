/*
 * xidos.h - top-level public header for the XiDOS runtime.
 *
 * Pulling in this header gives you the POSIX-ish surface XiDOS implements
 * on top of MS-DOS / FreeDOS. Individual headers (unistd.h, fcntl.h, ...)
 * may also be included directly.
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#ifndef XIDOS_H
#define XIDOS_H

#define XIDOS_VERSION_MAJOR 0
#define XIDOS_VERSION_MINOR 0
#define XIDOS_VERSION_PATCH 1
#define XIDOS_VERSION_STRING "0.0.1"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the runtime: fd table, standard streams, path/mount tables.
 * Called automatically from the startup code; safe to call again (no-op). */
void xidos_init(void);

/* Human-readable build/version string. */
const char *xidos_version(void);

#ifdef __cplusplus
}
#endif

#endif /* XIDOS_H */
