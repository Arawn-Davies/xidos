# XiDOS - top-level build.
#
# XiDOS is a *hybrid* runtime: a backend-neutral POSIX layer over one of two
# selectable backends.
#
#   BACKEND=realmode  (default) 16-bit real mode via INT 21h. v0 target:
#                     small utilities. Built with a 16-bit cross toolchain
#                     (default: Open Watcom V2).
#   BACKEND=pmode     32-bit protected mode on DJGPP. v1 target: makes
#                     bash / GCC / TCC feasible. Built with TOOLCHAIN=djgpp.
#
# The POSIX<->DOS path-conversion logic is host-buildable so it can be
# unit-tested with any C compiler, no DOS environment required.
#
# Usage:
#   make                       build libxidos.a with the default backend
#   make BACKEND=pmode         build against the pmode/DJGPP backend
#   make test                  build & run the host-side unit tests
#   make clean

BACKEND   ?= realmode
TOOLCHAIN ?= watcom

INCLUDES  := -Iinclude -Isrc

# Backend-neutral runtime sources (shared by every backend).
CORE_SRC := \
	src/init.c \
	src/io/fdtable.c \
	src/io/open.c \
	src/io/rdwr.c \
	src/path/pathconv.c \
	src/process/spawn.c

# Exactly one backend implementation is linked in.
BACKEND_SRC := src/backend/$(BACKEND)/$(BACKEND).c
ifeq ($(BACKEND),pmode)
BACKEND_SRC := src/backend/pmode/djgpp.c
endif

RUNTIME_SRC := $(CORE_SRC) $(BACKEND_SRC)

# --- DOS runtime (cross toolchain) ----------------------------------------
# Placeholder wiring: the real recipe depends on the chosen toolchain and is
# filled in as the backend INT 21h / DJGPP code paths land.
.PHONY: all
all: libxidos.a

libxidos.a: $(RUNTIME_SRC)
	@echo "Backend: $(BACKEND)   Toolchain: $(TOOLCHAIN)"
	@echo "TODO: compile with the $(TOOLCHAIN) toolchain:"
	@echo "  $(RUNTIME_SRC)"
	@echo "  (e.g. Open Watcom: wcc $(INCLUDES) ... && wlib -b libxidos.a +obj)"

# --- Host-side unit tests -------------------------------------------------
HOST_CC ?= cc

.PHONY: test
test: tests/test_pathconv
	./tests/test_pathconv

tests/test_pathconv: tests/test_pathconv.c src/path/pathconv.c
	$(HOST_CC) $(INCLUDES) -Wall -Wextra -o $@ \
		tests/test_pathconv.c src/path/pathconv.c

.PHONY: clean
clean:
	rm -f libxidos.a tests/test_pathconv
	find . -name '*.o' -delete 2>/dev/null || true
	find . -name '*.obj' -delete 2>/dev/null || true
