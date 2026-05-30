/*
 * realmode.c - the 16-bit real-mode XiDOS backend (INT 21h / BIOS).
 *
 * This implements the backend interface (src/sys/backend.h) directly on top
 * of DOS, talking to the kernel via INT 21h. It is the v0 target: enough to
 * run small POSIX-style utilities on any 8086+ in real mode. It deliberately
 * does NOT attempt bash/GCC-class workloads -- those need the 32-bit pmode
 * backend (flat memory + fork emulation).
 *
 * NOTE: the inline-asm INT 21h bodies are placeholders that document the
 * exact register conventions and currently fail with ENOSYS, so the rest of
 * the runtime can be built and unit-tested on a host before the real DOS
 * code paths are wired up per toolchain (Open Watcom / ia16 GCC).
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include "../../sys/backend.h"
#include <fcntl.h>
#include <errno.h>

/* The real-mode backend owns errno storage. The pmode/DJGPP backend will
 * instead use the underlying libc's errno and must NOT define this. */
int errno = 0;

/*
 * Map a DOS error code (returned in AX with CF set) onto a POSIX errno and
 * return -1, for the failure path of every wrapper below. See RBIL, INT 21h.
 */
static int fail_from_dos(unsigned dos_err)
{
    switch (dos_err) {
    case 0x01: errno = ENOSYS; break; /* invalid function number */
    case 0x02: errno = ENOENT; break; /* file not found */
    case 0x03: errno = ENOENT; break; /* path not found */
    case 0x04: errno = EMFILE; break; /* too many open files */
    case 0x05: errno = EACCES; break; /* access denied */
    case 0x06: errno = EBADF;  break; /* invalid handle */
    case 0x08: errno = ENOMEM; break; /* insufficient memory */
    case 0x0F: errno = EINVAL; break; /* invalid drive */
    case 0x10: errno = EACCES; break; /* cannot remove current directory */
    case 0x12: errno = ENOENT; break; /* no more files */
    default:   errno = EIO;    break;
    }
    return -1;
}

/* INT 21h AH=3Dh: open existing file. AL=access, DS:DX->ASCIIZ path. */
int xbe_open(const char *dospath, int flags, unsigned *handle)
{
    (void)dospath; (void)flags; (void)handle;
    return fail_from_dos(0x01); /* TODO: emit INT 21h AH=3Dh */
}

/* INT 21h AH=3Ch: create/truncate file. CX=attributes, DS:DX->path. */
int xbe_create(const char *dospath, int attr, unsigned *handle)
{
    (void)dospath; (void)attr; (void)handle;
    return fail_from_dos(0x01); /* TODO: emit INT 21h AH=3Ch */
}

/* INT 21h AH=3Eh: close handle. BX=handle. */
int xbe_close(unsigned handle)
{
    (void)handle;
    return fail_from_dos(0x01); /* TODO: emit INT 21h AH=3Eh */
}

/* INT 21h AH=3Fh: read. BX=handle, CX=count, DS:DX->buf; AX=bytes read. */
ssize_t xbe_read(unsigned handle, void *buf, size_t count)
{
    (void)handle; (void)buf; (void)count;
    return fail_from_dos(0x01); /* TODO: emit INT 21h AH=3Fh */
}

/* INT 21h AH=40h: write. BX=handle, CX=count, DS:DX->buf; AX=bytes written. */
ssize_t xbe_write(unsigned handle, const void *buf, size_t count)
{
    (void)handle; (void)buf; (void)count;
    return fail_from_dos(0x01); /* TODO: emit INT 21h AH=40h */
}

/* INT 21h AH=42h: lseek. AL=whence, BX=handle, CX:DX=offset; DX:AX=newpos. */
long xbe_lseek(unsigned handle, long offset, int whence)
{
    (void)handle; (void)offset; (void)whence;
    return fail_from_dos(0x01); /* TODO: emit INT 21h AH=42h */
}

/* INT 21h AH=41h: delete file. DS:DX->ASCIIZ path. */
int xbe_unlink(const char *dospath)
{
    (void)dospath;
    return fail_from_dos(0x01); /* TODO: emit INT 21h AH=41h */
}

/* INT 21h AH=4Bh AL=00h: EXEC (load & run child, blocking). No fork in real
 * mode -- the parent is suspended in place until the child exits. */
int xbe_spawn(const char *dospath, char *const argv[], char *const envp[])
{
    (void)dospath; (void)argv; (void)envp;
    return fail_from_dos(0x01); /* TODO: build EXEC param block, INT 21h AH=4Bh */
}
