/*
 * errno.h - POSIX error numbers (XiDOS).
 *
 * `errno` storage is provided by the linked backend: the real-mode backend
 * defines it and translates DOS error codes into these values; the
 * pmode/DJGPP backend forwards the host libc's errno. The DOS-error->errno
 * mapping itself is a backend-internal detail (see src/backend/realmode).
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#ifndef XIDOS_ERRNO_H
#define XIDOS_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

extern int errno;

#define EPERM    1   /* Operation not permitted */
#define ENOENT   2   /* No such file or directory */
#define EIO      5   /* I/O error */
#define EBADF    9   /* Bad file descriptor */
#define ENOMEM  12   /* Out of memory */
#define EACCES  13   /* Permission denied */
#define EEXIST  17   /* File exists */
#define EXDEV   18   /* Cross-device link */
#define ENOTDIR 20   /* Not a directory */
#define EISDIR  21   /* Is a directory */
#define EINVAL  22   /* Invalid argument */
#define EMFILE  24   /* Too many open files */
#define ENOSPC  28   /* No space left on device */
#define EROFS   30   /* Read-only file system */
#define ENOSYS  38   /* Function not implemented */

#ifdef __cplusplus
}
#endif

#endif /* XIDOS_ERRNO_H */
