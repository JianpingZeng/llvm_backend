#!/bin/bash -x

NAME=blah

CPU=CSE502

#CPU=i486

LIBC_PATH=../hello/musl-0.9.8
#LIBS=`echo $LIBC_PATH/lib/{crt1.o,libc.a}`
for c in *.c; do
	clang -Wno-main-return-type -emit-llvm -DENABLE_SCORE -DENABLE_PREVIEW -DENABLE_HIGH_SCORE -O3 -S -c -o $c.bc -nobuiltininc -isystem $LIBC_PATH/include $c
	#llc -march=x86 -mcpu=$CPU -O3 -view-legalize-dags -view-dag-combine1-dags -view-dag-combine2-dags -view-isel-dags -view-sched-dags $c.bc -o $c.S
	llc -march=x86 -mcpu=$CPU -O3 -view-sched-dags $c.bc -o $c.S
	as -o $c.o $c.S
done
as -o crt1.o crt1.s
ld -static -o $NAME *.o
