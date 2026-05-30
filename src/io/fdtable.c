/*
 * fdtable.c - implementation of the XiDOS file descriptor table.
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include "fdtable.h"
#include <errno.h>
#include <stddef.h>

static struct xidos_fd g_fds[XIDOS_OPEN_MAX];

void fdtable_init(void)
{
    int i;
    for (i = 0; i < XIDOS_OPEN_MAX; i++) {
        g_fds[i].kind = FD_FREE;
    }
    /* 0/1/2 map to the DOS standard handles, which share those numbers. */
    g_fds[0].kind = FD_TTY; g_fds[0].dos_handle = 0;
    g_fds[1].kind = FD_TTY; g_fds[1].dos_handle = 1;
    g_fds[2].kind = FD_TTY; g_fds[2].dos_handle = 2;
}

int fdtable_alloc(void)
{
    int i;
    for (i = 0; i < XIDOS_OPEN_MAX; i++) {
        if (g_fds[i].kind == FD_FREE) {
            return i;
        }
    }
    errno = EMFILE;
    return -1;
}

struct xidos_fd *fdtable_get(int fd)
{
    if (fd < 0 || fd >= XIDOS_OPEN_MAX || g_fds[fd].kind == FD_FREE) {
        errno = EBADF;
        return NULL;
    }
    return &g_fds[fd];
}

void fdtable_free(int fd)
{
    if (fd >= 0 && fd < XIDOS_OPEN_MAX) {
        g_fds[fd].kind = FD_FREE;
        g_fds[fd].dos_handle = 0;
        g_fds[fd].flags = 0;
    }
}
