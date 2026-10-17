# Build environment of LegacyProject.exe (see build.sh): Ubuntu 24.04 + MinGW-w64 GCC 13.2.0.
FROM ubuntu:24.04
RUN apt-get update && apt-get install -y --no-install-recommends g++-mingw-w64-x86-64-win32=13.2.0-6ubuntu1+26.1 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
CMD ["sh", "build.sh"]
