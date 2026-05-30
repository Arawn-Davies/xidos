/*
 * test_pathconv.c - host-buildable tests for POSIX <-> DOS path conversion.
 *
 * These tests don't touch DOS, so they compile and run on the build host
 * with any C compiler, e.g.:
 *   cc -I../include -I.. ../src/path/pathconv.c test_pathconv.c -o test_pathconv
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include <stdio.h>
#include <string.h>
#include "../src/path/pathconv.h"

static int failures = 0;

static void check_p2d(const char *in, const char *expect)
{
    char out[XIDOS_PATH_MAX];
    int rc = posix_to_dos_path(in, out, sizeof out);
    if (rc != 0 || strcmp(out, expect) != 0) {
        printf("FAIL posix_to_dos_path(\"%s\") -> \"%s\" (rc=%d), expected \"%s\"\n",
               in, rc == 0 ? out : "<err>", rc, expect);
        failures++;
    } else {
        printf("ok   \"%s\" -> \"%s\"\n", in, out);
    }
}

static void check_d2p(const char *in, const char *expect)
{
    char out[XIDOS_PATH_MAX];
    int rc = dos_to_posix_path(in, out, sizeof out);
    if (rc != 0 || strcmp(out, expect) != 0) {
        printf("FAIL dos_to_posix_path(\"%s\") -> \"%s\" (rc=%d), expected \"%s\"\n",
               in, rc == 0 ? out : "<err>", rc, expect);
        failures++;
    } else {
        printf("ok   \"%s\" -> \"%s\"\n", in, out);
    }
}

int main(void)
{
    check_p2d("/c/dos/file.txt", "C:\\dos\\file.txt");
    check_p2d("/c", "C:\\");
    check_p2d("foo/bar", "foo\\bar");

    check_d2p("C:\\DOS\\FILE.TXT", "/C/DOS/FILE.TXT");
    check_d2p("FOO\\BAR", "FOO/BAR");

    if (failures) {
        printf("\n%d test(s) failed.\n", failures);
        return 1;
    }
    printf("\nAll path conversion tests passed.\n");
    return 0;
}
