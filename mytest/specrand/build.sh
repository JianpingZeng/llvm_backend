#!/bin/bash -x

NAME=specrand

#PATH=~/llvm/built/Debug+Asserts/bin:$PATH

LIBC_PATH=/var/services/homes/spparmar/llvm/mytest/musl-1.0.4
LIBS=`echo $LIBC_PATH/lib/{crt1.o,libc.a,softfloat.o}`

for c in *.c; do
    echo "[Running] clang $c"
    clang -Wno-main-return-type -emit-llvm -DENABLE_SCORE -DENABLE_PREVIEW -DENABLE_HIGH_SCORE -O3 -S -c -o $c.bc -nobuiltininc -isystem $LIBC_PATH/include $c
    echo "[Running] llc $c.bc"
    #llc -soft-float -march=cse523 -O3 $c.bc -o $c.S
    ~/llvm/build/Debug+Asserts/bin/llc -soft-float -O3 $c.bc -o $c.S
    echo "[Running] as $c.S"
    as -o $c.o $c.S
done

echo "[Running] ld $NAME"
ld -static -o $NAME *.o $LIBS

