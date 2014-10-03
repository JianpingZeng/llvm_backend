#include "syscall.h"

short h = 6;
char hello[100] = "Hello World\n";
long main() {
    h = 3;
    hello[1] = 'E';
    __syscall3(__NR_write, 1, (long long) hello, 13);
    return 1;
}
