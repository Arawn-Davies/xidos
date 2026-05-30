---
title: Building
---

# Building XiDOS

XiDOS is built with a cross toolchain on a modern host and produces DOS binaries.

## Choosing a backend

XiDOS is hybrid (see [ARCHITECTURE.md](ARCHITECTURE.md)). Pick a backend with
`BACKEND=`:

- `BACKEND=realmode` (default) — 16-bit real mode via `INT 21h`. Build with a
  16-bit cross toolchain; the reference is Open Watcom V2.
- `BACKEND=pmode` — 32-bit protected mode. Build with `TOOLCHAIN=djgpp`. This is
  the backend that targets bash / GCC / TCC.

```sh
make                                   # realmode (default)
make BACKEND=pmode TOOLCHAIN=djgpp     # pmode on DJGPP
```

## Reference toolchain: Open Watcom V2 (realmode)

1. Install [Open Watcom V2](https://github.com/open-watcom/open-watcom-v2).
2. Source its environment (`owsetenv.sh` on Unix-like hosts) so `wcc`, `wlib`,
   and `wlink` are on your `PATH`.
3. From the repo root:

   ```sh
   make
   ```

   This produces `libxidos.a` (the runtime) and stages the public headers.

## Protected-mode toolchain: DJGPP in Docker (pmode)

There is no native DJGPP for Apple Silicon, and building the cross-GCC from
source is slow, so the toolchain lives in a small `linux/amd64` container
(emulated on arm64 via Docker Desktop / Rosetta). The container *compiles*;
DOSBox on the host *runs* the resulting `.EXE`.

```sh
make djgpp-image     # docker build -t xidos-djgpp tools/djgpp  (once)
```

The image carries the prebuilt DJGPP release (`i586-pc-msdosdjgpp-gcc`,
GCC 12.2.0) plus `CWSDPMI.EXE`. See `tools/djgpp/Dockerfile`.

## Testing in DOSBox

DJGPP programs are DPMI clients and plain DOSBox provides no DPMI host, so the
build stages `CWSDPMI.EXE` next to the program; the DJGPP stub loads it
automatically.

```sh
make dos          # build SMOKE.EXE, run it in DOSBox, capture & print C:\OUT.TXT
make dos-debug    # build, then drop into an interactive DOSBox shell (C: mounted)
```

`make dos` is the automated loop: DOSBox runs the program with stdout
redirected to `OUT.TXT`, exits, and the host prints the captured text — no
manual interaction. The DOSBox configs live in `tools/dosbox/`:

| File | Role |
|------|------|
| `dosbox-xidos.conf` | machine / CPU / video settings (empty autoexec) |
| `autoexec-run.conf` | automated: mount C:, run, capture, exit |
| `autoexec-debug.conf` | interactive: mount C:, drop to a prompt |

## Running the host unit tests

The POSIX<->DOS path logic is host-buildable, so it is unit-tested with the
host compiler — no DOS environment required:

```sh
make test
```

## Alternative toolchains

- **DJGPP** — a 32-bit protected-mode GCC port for DOS. Usable for the eventual
  pmode target; set `TOOLCHAIN=djgpp`.
- **GCC cross (ia16-elf)** — experimental 16-bit GCC. Not yet wired up.

## Notes

- The `Makefile` defaults to Open Watcom; override with `TOOLCHAIN=`.
- Real hardware testing is encouraged but optional. FreeDOS in a VM works well.
