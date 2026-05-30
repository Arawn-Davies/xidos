/*
 * pathconv.h - POSIX <-> DOS path translation.
 *
 * Cygwin-style mapping: a POSIX path like "/c/dos/file.txt" maps to the
 * DOS path "C:\DOS\FILE.TXT". A configurable mount/root table will later
 * let "/" point at a chosen drive and "/dev/..." map to DOS devices.
 *
 * Part of XiDOS (internal). Licensed under LGPL-2.1; see LICENSE.
 */
#ifndef XIDOS_PATH_PATHCONV_H
#define XIDOS_PATH_PATHCONV_H

#include <stddef.h>

/* DOS paths max out at 64 chars (drive + 63-char path) in classic DOS;
 * leave headroom. */
#define XIDOS_PATH_MAX 128

/* Convert a POSIX path to a DOS path into out[outsz].
 * Returns 0 on success, -1 if the result would not fit or is malformed. */
int posix_to_dos_path(const char *posix_path, char *out, size_t outsz);

/* Convert a DOS path back to POSIX form. Returns 0 / -1 as above. */
int dos_to_posix_path(const char *dos_path, char *out, size_t outsz);

#endif /* XIDOS_PATH_PATHCONV_H */
