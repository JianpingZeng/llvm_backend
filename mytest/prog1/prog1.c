#include "syscall.h"

long a = 10;
long b = 3;

long main()
{
	char* hello = "Hello world!\n";
        a = a - b;
	__syscall3(__NR_write, 1, (long long) hello, 13);
	return 0;
}
