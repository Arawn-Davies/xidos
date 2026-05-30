/*
 * backend.h - the XiDOS low-level backend interface.
 *
 * This is the seam that makes XiDOS a *hybrid* runtime. Everything above
 * this line (the fd table, open/read/write/close, path translation, the
 * eventual signal/fork emulation) is backend-neutral. Everything below it
 * is a concrete backend that knows how to talk to the machine:
 *
 *   - backend/realmode/  : 16-bit real mode, services via INT 21h / BIOS.
 *                          The v0 target. Small utilities only (640K, no
 *                          flat memory, no real fork()).
 *   - backend/pmode/     : 32-bit protected mode on top of a DPMI host,
 *                          initially backed by DJGPP's libc + extender.
 *                          The v1 target that makes bash / GCC / TCC
 *                          feasible (flat 32-bit memory, fork emulation).
 *
 * A build selects exactly one backend (see the Makefile's BACKEND var), so
 * these symbols resolve to one implementation at link time.
 *
 * Contract: each call returns 0 / a non-negative count on success, or -1
 * with `errno` set on failure. The backend owns the errno mapping, because
 * real mode must translate DOS error codes by hand while the pmode backend
 * can forward errno straight from the underlying libc.
 *
 * Part of XiDOS (internal). Licensed under LGPL-2.1; see LICENSE.
 */
#ifndef XIDOS_SYS_BACKEND_H
#define XIDOS_SYS_BACKEND_H

#include <sys/types.h>

/* `dospath` is an already-translated DOS path (see path/pathconv.h); the
 * backend does not do POSIX<->DOS translation itself. `flags` are the
 * fcntl.h O_* values; `handle` receives an opaque per-backend handle. */
int     xbe_open(const char *dospath, int flags, unsigned *handle);
int     xbe_create(const char *dospath, int attr, unsigned *handle);
int     xbe_close(unsigned handle);

ssize_t xbe_read(unsigned handle, void *buf, size_t count);
ssize_t xbe_write(unsigned handle, const void *buf, size_t count);
long    xbe_lseek(unsigned handle, long offset, int whence);

int     xbe_unlink(const char *dospath);

/* Block-and-run a child program. Returns its exit status (0..255) on
 * success, or -1 with errno set. Real mode uses INT 21h AH=4Bh (no fork);
 * the pmode backend will grow fork()/exec() emulation behind this and the
 * higher-level process API. */
int     xbe_spawn(const char *dospath, char *const argv[], char *const envp[]);

#endif /* XIDOS_SYS_BACKEND_H */
