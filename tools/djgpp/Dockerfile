# DJGPP cross-compiler for building XiDOS pmode (32-bit DOS) binaries.
#
# There is no native DJGPP toolchain for Apple Silicon, and building the
# cross-GCC from source is slow. So we run a small linux/amd64 container
# (emulated on arm64 via Docker Desktop / Rosetta) that carries the prebuilt
# DJGPP release. The container *compiles*; DOSBox on the host *runs* the
# resulting .EXE.
#
# DJGPP executables are DPMI clients and plain DOSBox provides no DPMI host,
# so we also stage CWSDPMI.EXE: the DJGPP stub loads it automatically when no
# DPMI host is present, as long as it sits next to (or on the PATH of) the
# program. The build copies it alongside the .EXE.
#
# Build:  docker build -t xidos-djgpp tools/djgpp
# Use:    docker run --rm -v "$PWD":/work xidos-djgpp \
#             i586-pc-msdosdjgpp-gcc -o build/dos/smoke.exe ...
#
# Part of XiDOS. The DJGPP toolchain is its own license; CWSDPMI is freely
# redistributable. This Dockerfile is LGPL-2.1; see LICENSE.
FROM --platform=linux/amd64 debian:bookworm-slim

ARG DJGPP_VERSION=v3.4
ARG DJGPP_TARBALL=djgpp-linux64-gcc1220.tar.bz2
ARG CSDPMI_URL=http://www.delorie.com/pub/djgpp/current/v2misc/csdpmi7b.zip

RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates curl bzip2 unzip make \
    && rm -rf /var/lib/apt/lists/*

# Prebuilt DJGPP cross toolchain -> /opt/djgpp/bin/i586-pc-msdosdjgpp-*
RUN curl -fSL \
        "https://github.com/andrewwutw/build-djgpp/releases/download/${DJGPP_VERSION}/${DJGPP_TARBALL}" \
        -o /tmp/djgpp.tar.bz2 \
    && tar -xjf /tmp/djgpp.tar.bz2 -C /opt \
    && rm /tmp/djgpp.tar.bz2

# CWSDPMI: the freely-redistributable DPMI host the DJGPP stub loads at
# runtime. DOSBox provides no DPMI, so this gets staged next to the program.
RUN mkdir -p /opt/cwsdpmi \
    && curl -fSL "${CSDPMI_URL}" -o /tmp/csdpmi.zip \
    && unzip -j /tmp/csdpmi.zip 'bin/CWSDPMI.EXE' -d /opt/cwsdpmi \
    && rm /tmp/csdpmi.zip \
    && test -f /opt/cwsdpmi/CWSDPMI.EXE

ENV PATH="/opt/djgpp/bin:${PATH}"
WORKDIR /work
