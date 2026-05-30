/*
 * rdwr.c - POSIX read()/write()/close()/lseek(), backend-neutral.
 *
 * These resolve the fd to its backend handle via the fd table and forward
 * to the linked backend, which sets errno on failure.
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include <unistd.h>
#include <errno.h>
#include "fdtable.h"
#include "../sys/backend.h"

ssize_t read(int fd, void *buf, size_t count)
{
    struct xidos_fd *slot = fdtable_get(fd);
    if (slot == NULL) {
        return -1; /* EBADF */
    }
    return xbe_read(slot->dos_handle, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    struct xidos_fd *slot = fdtable_get(fd);
    if (slot == NULL) {
        return -1; /* EBADF */
    }
    return xbe_write(slot->dos_handle, buf, count);
}

int close(int fd)
{
    struct xidos_fd *slot = fdtable_get(fd);
    int rc;
    if (slot == NULL) {
        return -1; /* EBADF */
    }
    rc = (slot->kind == FD_DOS_HANDLE) ? xbe_close(slot->dos_handle) : 0;
    fdtable_free(fd);
    return rc;
}

off_t lseek(int fd, off_t offset, int whence)
{
    struct xidos_fd *slot = fdtable_get(fd);
    if (slot == NULL) {
        return -1; /* EBADF */
    }
    if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END) {
        errno = EINVAL;
        return -1;
    }
    return (off_t)xbe_lseek(slot->dos_handle, offset, whence);
}
