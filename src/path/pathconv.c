/*
 * pathconv.c - POSIX <-> DOS path translation.
 *
 * Initial implementation handles the common cases:
 *   - "/c/foo/bar"  <-> "C:\FOO\BAR"  (leading "/<drive>/" form)
 *   - relative "foo/bar" <-> "FOO\BAR" (slash flipping only)
 * A configurable mount table is a planned enhancement.
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include "pathconv.h"

static int ch_is_drive_letter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static char to_upper(char c)
{
    return (c >= 'a' && c <= 'z') ? (char)(c - 'a' + 'A') : c;
}

int posix_to_dos_path(const char *posix_path, char *out, size_t outsz)
{
    size_t o = 0;
    const char *p = posix_path;

    if (posix_path == NULL || out == NULL || outsz == 0) {
        return -1;
    }

    /* Detect "/<drive>/..." and emit "<DRIVE>:\..." */
    if (p[0] == '/' && ch_is_drive_letter(p[1]) &&
        (p[2] == '/' || p[2] == '\0')) {
        if (outsz < 4) {
            return -1;
        }
        out[o++] = to_upper(p[1]);
        out[o++] = ':';
        out[o++] = '\\';
        p += (p[2] == '/') ? 3 : 2;
    }

    for (; *p; p++) {
        if (o + 1 >= outsz) {
            return -1;
        }
        out[o++] = (*p == '/') ? '\\' : *p;
    }
    out[o] = '\0';
    return 0;
}

int dos_to_posix_path(const char *dos_path, char *out, size_t outsz)
{
    size_t o = 0;
    const char *p = dos_path;

    if (dos_path == NULL || out == NULL || outsz == 0) {
        return -1;
    }

    /* Detect "<DRIVE>:\..." and emit "/<drive>/..." */
    if (ch_is_drive_letter(p[0]) && p[1] == ':') {
        if (outsz < 3) {
            return -1;
        }
        out[o++] = '/';
        out[o++] = to_upper(p[0]); /* keep case; callers may lowercase */
        p += 2;
        if (*p == '\\' || *p == '/') {
            p++;
        }
        if (o + 1 >= outsz) {
            return -1;
        }
        out[o++] = '/';
    }

    for (; *p; p++) {
        if (o + 1 >= outsz) {
            return -1;
        }
        out[o++] = (*p == '\\') ? '/' : *p;
    }
    out[o] = '\0';
    return 0;
}
