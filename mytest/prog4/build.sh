#!/bin/bash -x

NAME=prog4
#PATH=~/llvm/build/Debug+Asserts/bin:$PATH

for c in *.c; do
        echo "[Running] clang $c"
	clang -Wno-main-return-type -emit-llvm -nobuiltininc -msoft-float -O3 -S -c -o $c.bc $c
        echo "[Running] llc $c.bc"
	llc -march=cse523 -O3 $c.bc -o $c.S
        echo "[Running] as $c.S"
	as -o $c.o $c.S
done
echo "[Running] as crt1.s"
as -o crt1.o crt1.s
echo "[Running] ld $NAME"
ld -static -o $NAME *.o

