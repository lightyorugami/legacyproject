#!/bin/sh
# Builds LegacyProject.exe from LegacyProject.cpp, exactly as the official releases are built.
# Same compiler + same source = same file, byte for byte: compare the SHA-256 printed below with the one of the
# LegacyProject.exe of a release (certutil -hashfile LegacyProject.exe SHA256 on Windows).
#
# Compiler: MinGW-w64 GCC 13.2.0, package g++-mingw-w64-x86-64-win32 13.2.0-6ubuntu1+26.1 of Ubuntu 24.04.
# The easiest way to get exactly that one, on any system with Docker:
#     docker build -t lp-bootstrapper . && docker run --rm -v "$PWD:/src" lp-bootstrapper
# or in Ubuntu 24.04 (WSL on Windows works):
#     sudo apt install g++-mingw-w64-x86-64-win32 && sh build.sh
set -e
cd "$(dirname "$0")"
x86_64-w64-mingw32-g++-win32 -O2 -std=c++17 -municode -mwindows -static -static-libgcc -static-libstdc++ -s \
    -Wl,--no-insert-timestamp -o LegacyProject.exe LegacyProject.cpp
sha256sum LegacyProject.exe
