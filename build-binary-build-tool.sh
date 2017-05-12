#!/usr/bin/env bash

gcc -I ~/dev/binutils-gdb/binutils \
    -I ~/dev/binutils-gdb/include \
    -I /usr/include -L/usr/lib/x86_64-linux-gnu \
    -lbfd  binary-build-tool.c \
    -o binary-build-tool \
    ~/dev/binutils-gdb/bfd/libbfd.a  ~/dev/binutils-gdb/opcodes/.libs/libopcodes.a ~/dev/binutils-gdb/libiberty/libiberty.a \
    -ldl \
    -lz ~/dev/binutils-gdb/binutils/bucomm.o  \
    ~/dev/binutils-gdb/libiberty/make-temp-file.o \
    ~/dev/binutils-gdb/binutils/filemode.o
