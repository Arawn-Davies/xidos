/*
 * sys/types.h - primitive system data types (XiDOS).
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#ifndef XIDOS_SYS_TYPES_H
#define XIDOS_SYS_TYPES_H

#include <stddef.h>  /* size_t */

#ifndef _SSIZE_T_DEFINED
#define _SSIZE_T_DEFINED
typedef long ssize_t;
#endif

#ifndef _OFF_T_DEFINED
#define _OFF_T_DEFINED
typedef long off_t;    /* DOS files are limited to 2^31-1 bytes anyway */
#endif

typedef int    pid_t;
typedef unsigned short mode_t;
typedef unsigned short dev_t;
typedef unsigned short ino_t;
typedef short  nlink_t;
typedef unsigned short uid_t;  /* DOS has no users; reported as 0 */
typedef unsigned short gid_t;

#endif /* XIDOS_SYS_TYPES_H */
