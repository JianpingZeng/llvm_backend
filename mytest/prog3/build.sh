#!/bin/bash -x

NAME=prog3
PATH=/scratch/aakshintala/llvm/build/Debug+Asserts/bin:$PATH

#CPU=CSE502
CPU=corei7

as -o crt1.o crt1.s
for c in *.c; do
	clang -Wno-main-return-type -emit-llvm -nobuiltininc -msoft-float -g3 -O3 -S -c -o ${c%.c}.bc $c
	llc -view-sched-dags -march=x86-64 -mcpu=$CPU -O3 ${c%.c}.bc -o ${c%.c}.S
	as -o ${c%.c}.o ${c%.c}.S
done
ld -static -o $NAME *.o
objdump -d $NAME
