#!/bin/bash -x

NAME=prog2

CPU=CSE502
#CPU=corei7

# LIBC_PATH=./musl-0.9.8
#LIBS=`echo $LIBC_PATH/lib/{crt1.o,libc.a}`
as -o crt1.o crt1.s
for c in *.c; do
	clang -Wno-main-return-type -emit-llvm -nobuiltininc -msoft-float -O3 -S -c -o $c.bc $c
	llc -march=x86-64 -mcpu=$CPU -O3 $c.bc -o $c.S
	as -o $c.o $c.S
done
ld -static -o $NAME *.o



#NAME=prog2
#PATH=/scratch/mferdman/llvm-build/Debug+Asserts/bin:$PATH

#for i in *.c; do
#	clang -O3 -Wno-main-return-type -msoft-float -nobuiltininc -c -o ${i%.c}.o $i
#done
#as -o crt1.o crt1.s
#ld -static -o $NAME *.o
#objdump -D $NAME
