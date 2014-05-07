#include "syscall.h"

long main()
{
	char* hello = "Hello world!\n";
        __syscall3(__NR_write, 1, (long long) hello, 13);
	return 0;
}
