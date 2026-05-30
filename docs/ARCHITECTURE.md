---
title: Architecture
nav_order: 2
---

# XiDOS Architecture

XiDOS provides a POSIX-like programming interface on top of MS-DOS / FreeDOS.
This document describes the *intended* design; much of it is not yet implemented.

## The core idea, and why it's harder than Cygwin

Cygwin maps POSIX onto the Win32 API. The crucial thing it gets for free is a
real kernel underneath: NT already provides virtual memory, preemptive
multitasking, processes, threads, and a unified namespace. Cygwin is mostly a
*translation* layer.

DOS gives you almost none of that. Real-mode DOS is single-tasking, 16-bit,
limited to 640K of conventional memory, and has no notion of users, signals,
or `fork()`. So XiDOS cannot be a thin shim — it has to *supply* the kernel
services that Cygwin borrows from Windows. **XiDOS is less an API-translation
layer and more a minimal kernel + libc that happens to sit on DOS.**

(The inverse project, [dosix](https://github.com/oitofelix/dosix), runs DOS
*on* Linux and can afford to be thin precisely because Linux already has
everything. XiDOS faces the opposite, harder direction.)

## Prior art

This space is not empty; XiDOS should learn from it rather than reinvent it.

| Project | What it is | Relevance |
|---------|------------|-----------|
| **DJGPP** | 32-bit protected-mode GCC port for DOS with a large POSIX-ish libc and a DPMI/go32 extender. bash, make, emacs, gdb have been ported. | The closest existing thing to "GCC/bash on DOS." XiDOS's pmode backend stands on it. Notably it lacks a true `fork()`. |
| **EMX** | gcc runtime for DOS/OS2 in 32-bit pmode that *emulated `fork()`*. | Proof that fork emulation on DOS is feasible; a model for ours. |
| **Cygwin** | POSIX-on-Windows. | The licensing and path-mapping model XiDOS imitates. |
| **PDPCLIB / PDOS** | portable public-domain C library / OS. | Reference for a from-scratch libc. |
| **ELKS** | tiny real Unix kernel for the 8086. | Evidence 16-bit Unix-like behavior is achievable. |

The practical takeaway: "bash + a C compiler on DOS" already existed (DJGPP/EMX),
and both are **32-bit protected mode**. XiDOS's distinct value is the
Cygwin-style framing — a clean POSIX surface, `/c/`-style path mounts, proper
`fork()` — and a hybrid design that also serves tiny real-mode utilities.

## Hybrid design: one POSIX surface, two backends

The central architectural decision. Everything above the backend seam is
written once and is machine-independent; one concrete backend is linked in per
build (`BACKEND=` in the Makefile). The seam is `src/sys/backend.h`.

```
        Application (portable C; bash, tcc, coreutils-style tools)
                              |
        +-----------------------------------------------+
        |  XiDOS POSIX layer  (backend-neutral)         |
        |   fd table, open/read/write/close/lseek,      |
        |   POSIX<->DOS path translation, spawn/exec,   |
        |   (later) signals, fork emulation, termios    |
        +-----------------------------------------------+
                  |  src/sys/backend.h  (the seam)
        +------------------------+   +----------------------------+
        |  backend/realmode      |   |  backend/pmode             |
        |  16-bit, INT 21h/BIOS  |   |  32-bit pmode on DJGPP      |
        |  v0: small utilities   |   |  v1: bash / GCC / TCC       |
        +------------------------+   +----------------------------+
                  |                              |
              MS-DOS / FreeDOS            DPMI host (CWSDPMI/HDPMI)
                                                 + DJGPP libc/extender
```

### realmode backend (v0)
Talks to DOS directly via `INT 21h`. Targets any 8086+ in real mode. Good for
learning the DOS interface and shipping small POSIX-style utilities. Hard
limits: 640K, 16-bit pointers, no flat memory, no real `fork()`. bash and GCC
are **out of scope** here, by physics, not effort.

### pmode backend (v1)
The target that makes bash/GCC/TCC realistic, because they need a flat 32-bit
address space and megabytes of RAM. Rather than build a DPMI extender and a
32-bit libc from scratch, this backend stands on **DJGPP**: it forwards the
backend interface to DJGPP's mature libc/extender and lets XiDOS focus on what
DJGPP lacks. **TCC** is the most tractable compiler target to bring up first
(far smaller than GCC, plausibly self-hosting on DOS).

## Key sub-systems

### File descriptors vs. native handles
XiDOS keeps its own **fd table** (`src/io/fdtable.*`) so fds behave like POSIX
(0/1/2 reserved, lowest free fd reused, future `dup`/`dup2`), and so an fd can
later be backed by something that isn't a file (pipe, `/dev/null`, socket-like).
The table stores an opaque backend handle per fd.

### Path translation
`src/path/pathconv.*` converts between POSIX (`/c/foo/bar`, `/`-separated) and
DOS (`C:\FOO\BAR`, `\`-separated, drive-lettered) paths, Cygwin-style. A
configurable mount table (map `/` to a chosen drive, `/dev/...` to DOS devices)
is a planned enhancement. This logic is host-buildable and unit-tested
(`tests/test_pathconv.c`).

### errno
Backend-owned. The realmode backend translates DOS error codes (returned in AX
with CF set) into POSIX `errno` by hand and defines `errno` storage. The pmode
backend forwards the host libc's `errno` and does not define its own.

### Process model and `fork()`
This is the hardest problem and the gate on bash. DOS has no `fork()`.
- realmode: only blocking EXEC (`INT 21h` AH=4Bh) behind `xidos_spawn`; the
  parent is suspended until the child exits. No `fork()`.
- pmode: the plan is EMX/Cygwin-style `fork()` emulation — snapshot and copy the
  protected-mode address space, then resume both "processes" cooperatively. A
  real `fork()`/`exec()` API is layered above `xbe_spawn`. bash depends on this.

## Open questions
- Signals: which subset can be usefully emulated cooperatively in pmode?
- TTY/termios: how much line discipline is worth faking over the BIOS/DJGPP?
- Threads: out of scope for realmode; possible-but-deferred for pmode.
- Multitasking: DOS is single-tasking; any concurrency is cooperative and
  lives entirely inside the XiDOS pmode runtime.
