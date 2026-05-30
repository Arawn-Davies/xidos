/*
 * unistd.h - standard symbolic constants and POSIX I/O prototypes (XiDOS).
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#ifndef XIDOS_UNISTD_H
#define XIDOS_UNISTD_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Standard file descriptors. */
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* lseek() whence values. */
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

int     close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
off_t   lseek(int fd, off_t offset, int whence);
int     dup(int fd);
int     dup2(int oldfd, int newfd);
int     unlink(const char *path);
int     rmdir(const char *path);
int     chdir(const char *path);
char   *getcwd(char *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* XIDOS_UNISTD_H */
