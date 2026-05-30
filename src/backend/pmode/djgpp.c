/*
 * djgpp.c - the 32-bit protected-mode XiDOS backend (DJGPP-hosted).
 *
 * This is the v1 target: the one that makes bash, GCC and TCC realistic.
 * Real mode caps out at 640K and 16-bit pointers; bash/GCC need a flat
 * 32-bit address space and megabytes of RAM, which means protected mode on
 * top of a DPMI host (CWSDPMI / HDPMI). Rather than reimplement a DPMI
 * extender and a 32-bit libc from scratch, this backend stands on DJGPP:
 * it forwards the backend interface to DJGPP's already-mature libc and
 * go32 extender, and lets XiDOS concentrate on the parts DJGPP lacks --
 * notably real fork() emulation (EMX/Cygwin style, by copying the pmode
 * address space) and Cygwin-style "/c/..." path mounts.
 *
 * Because DJGPP's libc defines its own `errno`, this backend does NOT
 * define one (unlike the real-mode backend) and simply forwards it.
 *
 * STATUS: stub. Wiring this up requires building under the DJGPP toolchain
 * (TOOLCHAIN=djgpp); every entry point currently fails with ENOSYS.
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include "../../sys/backend.h"
#include <errno.h>

/*
 * Implementation sketch (per call): translate the XiDOS opaque handle to a
 * DJGPP file descriptor and forward to DJGPP libc (open/read/write/close/
 * lseek/unlink). DJGPP sets errno itself, so the wrappers just propagate
 * the return value. Process creation routes through the higher-level
 * fork()/exec() emulation that this backend will host.
 */

int xbe_open(const char *dospath, int flags, unsigned *handle)
{
    (void)dospath; (void)flags; (void)handle;
    errno = ENOSYS; return -1; /* TODO: forward to DJGPP open() */
}

int xbe_create(const char *dospath, int attr, unsigned *handle)
{
    (void)dospath; (void)attr; (void)handle;
    errno = ENOSYS; return -1; /* TODO: forward to DJGPP creat()/open() */
}

int xbe_close(unsigned handle)
{
    (void)handle;
    errno = ENOSYS; return -1; /* TODO: forward to DJGPP close() */
}

ssize_t xbe_read(unsigned handle, void *buf, size_t count)
{
    (void)handle; (void)buf; (void)count;
    errno = ENOSYS; return -1; /* TODO: forward to DJGPP read() */
}

ssize_t xbe_write(unsigned handle, const void *buf, size_t count)
{
    (void)handle; (void)buf; (void)count;
    errno = ENOSYS; return -1; /* TODO: forward to DJGPP write() */
}

long xbe_lseek(unsigned handle, long offset, int whence)
{
    (void)handle; (void)offset; (void)whence;
    errno = ENOSYS; return -1; /* TODO: forward to DJGPP lseek() */
}

int xbe_unlink(const char *dospath)
{
    (void)dospath;
    errno = ENOSYS; return -1; /* TODO: forward to DJGPP unlink() */
}

int xbe_spawn(const char *dospath, char *const argv[], char *const envp[])
{
    (void)dospath; (void)argv; (void)envp;
    errno = ENOSYS; return -1; /* TODO: route via fork()/exec() emulation */
}
