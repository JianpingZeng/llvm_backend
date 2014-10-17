#include "syscall.h"

char h1 = 'E', h2;
int h3 = 1;
char hello[100] = "Hello World\n";
long main() {
    h2 = h3 + h1;
    hello[1] = h2;
    __syscall3(__NR_write, 1, (long long) hello, 13);
    return 1;
}
