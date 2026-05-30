/*
 * init.c - XiDOS runtime initialization and version reporting.
 *
 * Part of XiDOS. Licensed under LGPL-2.1; see LICENSE.
 */
#include <xidos.h>
#include "io/fdtable.h"

static int g_initialized = 0;

void xidos_init(void)
{
    if (g_initialized) {
        return;
    }
    fdtable_init();
    g_initialized = 1;
}

const char *xidos_version(void)
{
    return "XiDOS " XIDOS_VERSION_STRING;
}
