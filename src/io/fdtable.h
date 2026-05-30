/*
 * fdtable.h - POSIX file descriptor table backed by DOS handles.
 *
 * XiDOS keeps its own fd table so descriptors behave like POSIX (lowest
 * free fd reused, 0/1/2 reserved, dup/dup2 semantics) and so an fd can be
 * backed by something other than a raw DOS handle in the future.
 *
 * Part of XiDOS (internal). Licensed under LGPL-2.1; see LICENSE.
 */
#ifndef XIDOS_IO_FDTABLE_H
#define XIDOS_IO_FDTABLE_H

#define XIDOS_OPEN_MAX 32

enum xidos_fd_kind {
    FD_FREE = 0,
    FD_DOS_HANDLE,  /* backed by a DOS file handle */
    FD_TTY          /* console device */
};

struct xidos_fd {
    enum xidos_fd_kind kind;
    unsigned dos_handle;   /* valid when kind == FD_DOS_HANDLE */
    int      flags;        /* open() flags */
};

/* Allocate the lowest free fd >= 0, returning it, or -1 (errno=EMFILE). */
int  fdtable_alloc(void);

/* Look up an fd; returns NULL and sets errno=EBADF if invalid. */
struct xidos_fd *fdtable_get(int fd);

/* Release an fd back to the table. */
void fdtable_free(int fd);

/* Set up entries 0/1/2 for the standard streams. */
void fdtable_init(void);

#endif /* XIDOS_IO_FDTABLE_H */
