#include "syscall.h"

char hello[100] = "Hello World\n";
int main() {
    __syscall3(__NR_write, 1, (long long) hello, 13);
    return 0;
}
