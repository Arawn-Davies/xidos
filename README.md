# XiDOS

**A POSIX compatibility layer for MS-DOS / FreeDOS.**

XiDOS is to DOS what [Cygwin](https://www.cygwin.com/) is to Windows: a runtime
library plus a set of headers that implement a substantial subset of the POSIX
API on top of the DOS kernel. Where Cygwin maps POSIX semantics onto the Win32
API, XiDOS maps them onto the MS-DOS / FreeDOS system call interface (chiefly
`INT 21h`) and the underlying BIOS.

The goal is to let software written for Unix-like systems compile and run on real
(or emulated) DOS machines with minimal changes — `open`/`read`/`write`/`close`,
path translation between POSIX (`/usr/bin`) and DOS (`C:\USR\BIN`) conventions,
`errno`, process spawning, and a growing slice of the C library that DOS does not
provide natively.

> ⚠️ **Status: early scaffolding.** The API surface is being stubbed out. Nothing
> here is production-ready yet. See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
> for the design and [the roadmap](#roadmap) for what is and isn't implemented.

## Why?

DOS is a single-tasking, real-mode operating system with no notion of users,
processes, signals, or a unified filesystem namespace. A huge amount of useful,
portable C code assumes all of those things exist. XiDOS bridges that gap so the
two can meet, in the same spirit that Cygwin and MinGW bridged Unix and Windows.

| | Host OS | API translated to | Analogy |
|------------------|------------------|-------------------|---------|
| Cygwin           | Windows (NT)     | Win32             | full POSIX emulation |
| MinGW            | Windows (NT)     | Win32 (thin)      | native, minimal layer |
| DJGPP            | DOS (32-bit pmode) | DPMI + libc     | GCC port |
| **XiDOS**        | **MS-DOS / FreeDOS** | **INT 21h / BIOS** | **POSIX emulation** |

## Hybrid design: one POSIX surface, two backends

DOS in real mode caps out at 640K and 16-bit pointers — fine for small tools,
but bash and GCC need a flat 32-bit address space. XiDOS resolves this with a
**backend seam** ([`src/sys/backend.h`](src/sys/backend.h)): the whole POSIX
layer is written once and machine-independent, and one backend is linked in per
build.

- **`realmode`** (v0) — 16-bit, services via `INT 21h`/BIOS. Small POSIX-style
  utilities on any 8086+. No flat memory, no real `fork()`.
- **`pmode`** (v1) — 32-bit protected mode on top of [DJGPP](https://www.delorie.com/djgpp/)
  (DPMI host + libc/extender). The target that makes **bash / GCC / TCC**
  feasible, plus EMX/Cygwin-style `fork()` emulation. TCC is the first compiler
  to bring up — far smaller than GCC.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for the full design.

## Building

XiDOS targets DOS but is built with a cross toolchain. The reference toolchain
for the realmode backend is [Open Watcom V2](https://github.com/open-watcom/open-watcom-v2);
the pmode backend builds under DJGPP.

```sh
make                    # build libxidos.a, default realmode backend
make BACKEND=pmode TOOLCHAIN=djgpp   # build the 32-bit pmode backend
make test               # build & run host-side unit tests (any C compiler)
make clean
```

See [docs/BUILDING.md](docs/BUILDING.md) for toolchain setup.

## Layout

```
include/     Public POSIX-ish headers (unistd.h, fcntl.h, errno.h, sys/*.h)
src/
  io/        fd table + open/read/write/close/lseek (backend-neutral)
  path/      POSIX <-> DOS path translation (host-testable)
  process/   spawn/exec; fork() emulation lives in the pmode backend
  sys/       backend.h — the seam between the POSIX layer and the machine
  backend/
    realmode/  16-bit INT 21h / BIOS backend (v0)
    pmode/     32-bit DJGPP-hosted backend (v1)
docs/        Design and build documentation
tests/       Unit tests (path conversion is host-buildable)
```

## Prior art

"bash + a C compiler on DOS" already exists, and XiDOS builds on those lessons:
[DJGPP](https://www.delorie.com/djgpp/) (32-bit GCC + POSIX-ish libc; no true
`fork`), [EMX](https://en.wikipedia.org/wiki/EMX_(programming_environment))
(DOS/OS2 gcc runtime that *emulated* `fork()`), and of course
[Cygwin](https://www.cygwin.com/) on Windows. The reverse direction —
DOS-on-Linux — is done by [dosix](https://github.com/oitofelix/dosix). XiDOS's
distinct angle is a clean Cygwin-style POSIX surface with `/c/`-style path
mounts and real `fork()`, in a hybrid that also serves tiny real-mode tools.
See [docs/ARCHITECTURE.md#prior-art](docs/ARCHITECTURE.md) for the full survey.

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

Distributed under the GNU Lesser General Public License v2.1 (LGPL-2.1). The
LGPL is used (rather than the plain GPL) so that programs compiled against the
XiDOS runtime are not themselves forced to be GPL — only modifications to XiDOS
itself must stay open. This mirrors how glibc and the Cygwin runtime are
licensed, but stays in the GPL-2 family. See [LICENSE](LICENSE).

## Contributing

This is an experimental hobby project. Issues and PRs welcome — please read the
architecture doc first so changes fit the intended design.
