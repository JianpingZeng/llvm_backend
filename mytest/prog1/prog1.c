#include "syscall.h"

int h = 6;
char hello[100] = "Hello World\n";
long main() {
    h = 'E';
    hello[1] = h;
    __syscall3(__NR_write, 1, (long long) hello, 13);
    return 1;
}
