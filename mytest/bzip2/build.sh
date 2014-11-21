#!/bin/bash -x

NAME=bzip2

#PATH=~/llvm/built/Debug+Asserts/bin:$PATH

LIBC_PATH=/var/services/homes/spparmar/musl-1.0.4
LIBS=`echo $LIBC_PATH/lib/{crt1.o,libc.a,softfloat.o}`

for c in *.c; do
#for c in compress.c; do
#for c in spec.c randtable.c blocksort.c crctable.c huffman.c decompress.c bzip2.c; do
    #echo "[Running] gcc $c"
    #gcc -nostdinc -c -o $c.o -I$LIBC_PATH/include $c
    echo "[Running] clang $c"
    clang -Wno-main-return-type -emit-llvm -DENABLE_SCORE -DENABLE_PREVIEW -DENABLE_HIGH_SCORE -O3 -S -c -o $c.bc -nobuiltininc -isystem $LIBC_PATH/include $c
    echo "[Running] llc $c.bc"
    llc -soft-float -march=cse523 -O3 $c.bc -o $c.S
    #~/llvm/build/Debug+Asserts/bin/llc -soft-float -O3 $c.bc -o $c.S
    echo "[Running] as $c.S"
    as -o $c.o $c.S
done
#echo "[Running] as crt1.s"
#as -o crt1.o crt1.s
echo "[Running] ld $NAME"
ld -static -o $NAME *.o $LIBS

