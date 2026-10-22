#!/bin/bash

# this script needs c2 (peephole optimizer)
# to setup c2:
# $ /opt/v4/sys/bin/cc /opt/v4/sys/usr/c/c20.c
# $ sudo mv a.out /opt/v4/sys/lib/c2

mkdir -p example/bin
/opt/v4/sys/bin/cc -O example/char.c
mv a.out example/bin/char
/opt/v4/sys/bin/cc -O example/crash.c
mv a.out example/bin/crash
/opt/v4/sys/bin/cc -O example/crypttest.c example/allocator.c
mv a.out example/bin/crypttest
/opt/v4/sys/bin/cc -O example/alloctest.c example/allocator.c example/io.c
mv a.out example/bin/alloctest
/opt/v4/sys/bin/cc -O example/float.c
mv a.out example/bin/float
/opt/v4/sys/bin/cc -O example/hello.c
mv a.out example/bin/hello
