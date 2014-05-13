#!/bin/bash -x

NAME=prog2

#PATH=~/llvm/built/Debug+Asserts/bin:$PATH

LIBC_PATH=./musl-0.9.8
#LIBS=`echo $LIBC_PATH/lib/{crt1.o,libc.a}`
for c in *.c; do
        echo "[Running] clang $c"
	clang -Wno-main-return-type -emit-llvm -DENABLE_SCORE -DENABLE_PREVIEW -DENABLE_HIGH_SCORE -O3 -S -c -o $c.bc -nobuiltininc -isystem $LIBC_PATH/include $c
        echo "[Running] llc $c.bc"
	llc -march=cse523 -O3 $c.bc -o $c.S
        echo "[Running] as $c.S"
	as -o $c.o $c.S
done
echo "[Running] as crt1.s"
as -o crt1.o crt1.s
echo "[Running] ld $NAME"
ld -static -o $NAME *.o

