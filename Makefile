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

# --- DJGPP (pmode) cross build via Docker ---------------------------------
# There is no native DJGPP toolchain for Apple Silicon, so the cross compiler
# lives in a container (see tools/djgpp/Dockerfile). These targets build a DOS
# .EXE and drive DOSBox so the whole edit->build->run loop is one command.
DOCKER      ?= docker
DJGPP_IMAGE ?= xidos-djgpp
DJGPP_CC    := i586-pc-msdosdjgpp-gcc
DOSDIR      := build/dos
DOSBOX      ?= /Applications/dosbox.app/Contents/MacOS/DOSBox
DOSBOX_CONF := $(CURDIR)/tools/dosbox/dosbox-xidos.conf

# Run a command inside the DJGPP toolchain container, repo mounted at /work.
DJGPP_RUN = $(DOCKER) run --rm -v "$(CURDIR)":/work -w /work $(DJGPP_IMAGE)

.PHONY: djgpp-image
djgpp-image:
	$(DOCKER) build -t $(DJGPP_IMAGE) tools/djgpp

$(DOSDIR):
	mkdir -p $(DOSDIR)

# DOS smoke-test EXE. Built with DJGPP; CWSDPMI.EXE (the DPMI host the DJGPP
# stub needs at runtime, which DOSBox does not provide) is staged next to it.
$(DOSDIR)/SMOKE.EXE: tests/dos_smoke.c src/path/pathconv.c | $(DOSDIR)
	$(DJGPP_RUN) $(DJGPP_CC) -O2 -Wall -Iinclude -Isrc \
		-o $(DOSDIR)/SMOKE.EXE tests/dos_smoke.c src/path/pathconv.c
	$(DJGPP_RUN) sh -c 'cp /opt/cwsdpmi/CWSDPMI.EXE "$(DOSDIR)/CWSDPMI.EXE"'

.PHONY: dos-smoke
dos-smoke: $(DOSDIR)/SMOKE.EXE

# Automated: build, run under DOSBox capturing output to OUT.TXT, print it.
.PHONY: dos
dos: dos-smoke
	cd $(DOSDIR) && "$(DOSBOX)" \
		-conf "$(DOSBOX_CONF)" \
		-conf "$(CURDIR)/tools/dosbox/autoexec-run.conf"
	@echo "----- SMOKE.EXE output (C:\\OUT.TXT) -----"
	@cat $(DOSDIR)/OUT.TXT 2>/dev/null || echo "(no OUT.TXT - did SMOKE.EXE run?)"

# Interactive: build, then drop into a DOSBox shell with C: mounted.
.PHONY: dos-debug
dos-debug: dos-smoke
	cd $(DOSDIR) && "$(DOSBOX)" \
		-conf "$(DOSBOX_CONF)" \
		-conf "$(CURDIR)/tools/dosbox/autoexec-debug.conf"

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
	rm -rf build
	rm -f libxidos.a tests/test_pathconv
	find . -name '*.o' -delete 2>/dev/null || true
	find . -name '*.obj' -delete 2>/dev/null || true
