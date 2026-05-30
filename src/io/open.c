/*
 * open.c - POSIX open()/creat(), backend-neutral.
 *
 * Allocates an fd, translates the POSIX path to DOS form, then hands the
 * actual open to whichever backend is linked in (real-mode INT 21h or
 * pmode/DJGPP). The fd table maps the POSIX fd onto the backend handle.
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include <fcntl.h>
#include <errno.h>
#include "fdtable.h"
#include "../path/pathconv.h"
#include "../sys/backend.h"

int open(const char *path, int flags, ...)
{
    char dospath[XIDOS_PATH_MAX];
    int fd;
    unsigned handle;
    struct xidos_fd *slot;

    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (posix_to_dos_path(path, dospath, sizeof dospath) != 0) {
        errno = ENOENT;
        return -1;
    }

    fd = fdtable_alloc();
    if (fd < 0) {
        return -1; /* errno = EMFILE */
    }

    if (flags & O_CREAT) {
        if (xbe_create(dospath, 0 /* normal attr */, &handle) != 0) {
            fdtable_free(fd);
            return -1; /* backend set errno */
        }
    } else {
        if (xbe_open(dospath, flags, &handle) != 0) {
            fdtable_free(fd);
            return -1; /* backend set errno */
        }
    }

    slot = fdtable_get(fd);
    slot->kind = FD_DOS_HANDLE;
    slot->dos_handle = handle;
    slot->flags = flags;
    return fd;
}

int creat(const char *path, int mode)
{
    (void)mode; /* DOS attribute mapping handled inside the backend */
    return open(path, O_CREAT | O_WRONLY | O_TRUNC);
}
