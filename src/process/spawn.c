/*
 * spawn.c - process creation for XiDOS.
 *
 * DOS is single-tasking and has no fork(). XiDOS exposes a posix_spawn-style
 * call backed by the linked backend:
 *   - real mode: INT 21h AH=4Bh EXEC, which suspends the parent until the
 *     child exits. True fork() is not offered here.
 *   - pmode/DJGPP: the backend will host EMX/Cygwin-style fork() emulation
 *     (copying the protected-mode address space); a real fork()/exec() API
 *     can then be layered above this entry point. That is what bash needs.
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include <errno.h>
#include "../path/pathconv.h"
#include "../sys/backend.h"

int xidos_spawn(const char *path, char *const argv[], char *const envp[])
{
    char dospath[XIDOS_PATH_MAX];

    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (posix_to_dos_path(path, dospath, sizeof dospath) != 0) {
        errno = ENOENT;
        return -1;
    }
    return xbe_spawn(dospath, argv, envp);
}
