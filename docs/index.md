---
title: Home
nav_order: 1
---

# XiDOS

**A POSIX compatibility layer for MS-DOS / FreeDOS.**

XiDOS is to DOS what [Cygwin](https://www.cygwin.com/) is to Windows: a runtime
library plus headers that implement a substantial subset of the POSIX API on top
of the DOS kernel. Where Cygwin maps POSIX semantics onto the Win32 API, XiDOS
maps them onto the MS-DOS / FreeDOS system-call interface (chiefly `INT 21h`) and
the underlying BIOS.

The goal is to let Unix-flavoured C compile and run on real (or emulated) DOS
machines with minimal changes — `open`/`read`/`write`/`close`, path translation
between POSIX (`/usr/bin`) and DOS (`C:\USR\BIN`) conventions, `errno`, process
spawning, and a growing slice of the C library that DOS does not provide.

> ⚠️ **Status: early scaffolding.** The API surface is being stubbed out. Nothing
> here is production-ready yet.

## Documentation

- **[Architecture](ARCHITECTURE.md)** — the hybrid design, the backend seam, the
  process/`fork()` problem, and a survey of prior art (DJGPP, EMX, Cygwin).
- **[Building](BUILDING.md)** — toolchain setup for the real-mode (Open Watcom)
  and pmode (DJGPP-in-Docker) backends, plus the DOSBox run/debug harness.

The source lives at
[github.com/Arawn-Davies/xidos](https://github.com/Arawn-Davies/xidos).

## Hybrid design in one picture

DOS in real mode caps out at 640K and 16-bit pointers — fine for small tools, but
bash and GCC need a flat 32-bit address space. XiDOS resolves this with a
**backend seam**: the whole POSIX layer is written once and machine-independent,
and exactly one backend is linked in per build.

| | Host OS | API translated to | Analogy |
|------------------|----------------------|--------------------|----------------------|
| Cygwin           | Windows (NT)         | Win32              | full POSIX emulation |
| MinGW            | Windows (NT)         | Win32 (thin)       | native, minimal layer |
| DJGPP            | DOS (32-bit pmode)   | DPMI + libc        | GCC port |
| **XiDOS**        | **MS-DOS / FreeDOS** | **INT 21h / BIOS** | **POSIX emulation** |

- **`realmode`** (v0) — 16-bit, services via `INT 21h`/BIOS. Small POSIX-style
  utilities on any 8086+. No flat memory, no real `fork()`.
- **`pmode`** (v1) — 32-bit protected mode on top of
  [DJGPP](https://www.delorie.com/djgpp/). The target that makes
  **bash / GCC / TCC** feasible, plus EMX/Cygwin-style `fork()` emulation.

See the [architecture doc](ARCHITECTURE.md) for the full design.

## Roadmap

**v0 — realmode backend (small utilities)**

- [x] Project scaffold, hybrid build system, backend seam
- [x] POSIX ↔ DOS path translation (`/c/foo` ↔ `C:\FOO`), host-tested
- [x] fd table mapping POSIX fds to backend handles
- [ ] `open`/`read`/`write`/`close`/`lseek` over `INT 21h`
- [ ] `errno` mapping from DOS error codes
- [ ] Directory operations (`opendir`/`readdir`)
- [ ] Process spawning via `INT 21h` AH=4Bh (blocking `spawn`, no `fork`)
- [ ] Standard streams and basic TTY handling
- [ ] A `mount`-style table for the virtual filesystem root

**v1 — pmode backend (bash / GCC / TCC)**

- [ ] DJGPP-hosted backend wiring (forward `xbe_*` to DJGPP libc)
- [ ] `fork()` emulation (EMX/Cygwin-style address-space copy)
- [ ] `exec*` family + environment passing
- [ ] Bring up **TCC**, then a shell (bash) on top of XiDOS
- [ ] Signals subset, termios subset

## License

Distributed under the GNU Lesser General Public License v2.1 (LGPL-2.1), so that
programs compiled against the XiDOS runtime are not themselves forced to be GPL —
only modifications to XiDOS itself must stay open. This mirrors how glibc and the
Cygwin runtime are licensed, but stays in the GPL-2 family.
