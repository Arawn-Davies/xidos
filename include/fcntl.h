/*
 * fcntl.h - file control options (XiDOS).
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#ifndef XIDOS_FCNTL_H
#define XIDOS_FCNTL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Access modes (mutually exclusive in the low two bits). */
#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR   0x0002
#define O_ACCMODE 0x0003

/* Open-time flags. */
#define O_CREAT  0x0100
#define O_EXCL   0x0200
#define O_TRUNC  0x0400
#define O_APPEND 0x0800
#define O_BINARY 0x8000  /* DOS distinguishes text vs binary; default binary */
#define O_TEXT   0x4000

int open(const char *path, int flags, ...);
int creat(const char *path, int mode);

#ifdef __cplusplus
}
#endif

#endif /* XIDOS_FCNTL_H */
