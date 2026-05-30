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

## Running the tests

Tests are compiled to DOS executables and run under an emulator. If
[DOSBox](https://www.dosbox.com/) (or DOSBox-X) is installed and on your `PATH`,
`make test` will invoke it automatically; otherwise it prints instructions for
running the test binaries manually.

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
