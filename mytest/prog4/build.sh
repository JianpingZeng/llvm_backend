#!/bin/bash

NAME=prog4
PATH=../../build/Debug+Asserts/bin:$PATH

#CPU=corei7
CPU=CSE502

as -o crt1.o crt1.s
for c in *.c; do
	clang -Wno-main-return-type -emit-llvm -nobuiltininc -msoft-float -O3 -S -c -o ${c%.c}.bc $c
	llc -march=x86-64 -mcpu=$CPU -O3 ${c%.c}.bc -o ${c%.c}.S
	as -o ${c%.c}.o ${c%.c}.S
done
ld -static -o $NAME *.o
./$NAME 3
