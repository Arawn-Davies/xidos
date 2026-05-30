/*
 * dos_smoke.c - a tiny end-to-end smoke test for the DOSBox run loop.
 *
 * It deliberately exercises only the parts of XiDOS that are real today and
 * have no dependency on the (still-stubbed) INT 21h backend: the version
 * string and the POSIX<->DOS path translator. That keeps it buildable both
 * on the host (any C compiler) and for DOS (Open Watcom), so a successful
 * run prints identical, visible output in the DOSBox window.
 *
 * Build for DOS:  wcl -bcl=dos -ms dos_smoke.c ../src/path/pathconv.c
 * Build on host:  cc -Iinclude -Isrc tests/dos_smoke.c src/path/pathconv.c
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include <stdio.h>
#include "../include/xidos.h"
#include "../src/path/pathconv.h"

static void show_p2d(const char *posix)
{
    char dos[XIDOS_PATH_MAX];
    if (posix_to_dos_path(posix, dos, sizeof dos) == 0) {
        printf("  %-18s -> %s\n", posix, dos);
    } else {
        printf("  %-18s -> <error>\n", posix);
    }
}

static void show_d2p(const char *dos)
{
    char posix[XIDOS_PATH_MAX];
    if (dos_to_posix_path(dos, posix, sizeof posix) == 0) {
        printf("  %-18s -> %s\n", dos, posix);
    } else {
        printf("  %-18s -> <error>\n", dos);
    }
}

int main(void)
{
    printf("=== XiDOS %s smoke test ===\n", XIDOS_VERSION_STRING);
    printf("Running on DOS via the XiDOS runtime.\n\n");

    printf("POSIX -> DOS path translation:\n");
    show_p2d("/c/dos/file.txt");
    show_p2d("/c");
    show_p2d("foo/bar");

    printf("\nDOS -> POSIX path translation:\n");
    show_d2p("C:\\DOS\\FILE.TXT");
    show_d2p("FOO\\BAR");

    printf("\nSmoke test OK.\n");
    return 0;
}
