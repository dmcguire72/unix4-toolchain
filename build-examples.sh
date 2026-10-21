#!/bin/bash

mkdir -p example/bin
/opt/v4/sys/bin/cc example/char.c
mv a.out example/bin/char
/opt/v4/sys/bin/cc example/crash.c
mv a.out example/bin/crash
/opt/v4/sys/bin/cc example/crypttest.c example/allocator.c
mv a.out example/bin/crypttest
/opt/v4/sys/bin/cc example/alloctest.c example/allocator.c
mv a.out example/bin/alloctest
/opt/v4/sys/bin/cc example/float.c
mv a.out example/bin/float
/opt/v4/sys/bin/cc example/hello.c
mv a.out example/bin/hello
