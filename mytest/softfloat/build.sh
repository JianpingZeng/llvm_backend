#!/bin/bash -x

NAME=softfloat

#gcc -O2 -c $NAME.c -o $NAME.o
echo "[Running] clang $NAME.c"
clang -Wno-main-return-type -emit-llvm -DENABLE_SCORE -DENABLE_PREVIEW -DENABLE_HIGH_SCORE -O3 -S -c -o $NAME.bc -nobuiltininc -isystem $LIBC_PATH/include $NAME.c
echo "[Running] llc $NAME.bc"
llc -march=cse523 -O3 $NAME.bc -o $NAME.S
#~/llvm/build/Debug+Asserts/bin/llc -O3 $NAME.bc -o $NAME.S
echo "[Running] as $NAME.S"
as -o $NAME.o $NAME.S
