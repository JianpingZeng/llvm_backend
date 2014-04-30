#include "libc.h"
#include "syscall.h"

typedef long long INT;

void llexit(long long s) {
	__syscall1(__NR_exit, s);
}

size_t llstrlen(const char *s) {
	INT l;
	unsigned long long v = (unsigned long long)s;
	unsigned long long *p = (unsigned long long *)(v & ~7);
	l = -(v % 8);
	switch(-l) {
		case 0: do {
		if ((*p & 0x00000000000000ff) == 0) return l;
		case 1:
		if ((*p & 0x000000000000ff00) == 0) return l+1;
		case 2:
		if ((*p & 0x0000000000ff0000) == 0) return l+2;
		case 3:
		if ((*p & 0x00000000ff000000) == 0) return l+3;
		case 4:
		if ((*p & 0x000000ff00000000) == 0) return l+4;
		case 5:
		if ((*p & 0x0000ff0000000000) == 0) return l+5;
		case 6:
		if ((*p & 0x00ff000000000000) == 0) return l+6;
		default: // 7
		if ((*p & 0xff00000000000000) == 0) return l+7;
		l += 8;
		++p; } while(1);
	}
	// unreachable
}

INT puts(const char *s) {
	return __syscall3(__NR_write, 1, (long long)s, strlen(s));
}

INT putchar(INT c) {
	INT s[1];
	s[0] = c;
	return __syscall3(__NR_write, 1, (long long)s, 1);
}

char* current_brk;
void *malloc(unsigned long size) {
	size = ((size-1)/8+1)*8;
	current_brk += size;
	__syscall1(__NR_brk, (long long)current_brk);
	return (current_brk-size);
}

// from musl
INT isspace(INT c)
{
	return c == ' ' || (unsigned long long)c-'\t' < 5;
}

INT isdigit(INT c)
{
	return (unsigned long long)c-'0' < 10;
}

INT LLreadchar(const char *c) {
	unsigned long long r;
	unsigned long long v = (unsigned long long)c;
	unsigned long long *p = (unsigned long long *)(v & ~7);
	r = (*p);
	switch(v & 7) {
	case 0: r = r >> (0 * 8); break;
	case 1: r = r >> (1 * 8); break;
	case 2: r = r >> (2 * 8); break;
	case 3: r = r >> (3 * 8); break;
	case 4: r = r >> (4 * 8); break;
	case 5: r = r >> (5 * 8); break;
	case 6: r = r >> (6 * 8); break;
	case 7: r = r >> (7 * 8); break;
	}
	return (r & 0xff);
}

long atol(const char *s)
{
	long long n=0;
	INT neg=0;
	while (isspace(LLreadchar(s))) s++;
	switch (LLreadchar(s)) {
	case '-': neg=1;
	case '+': s++;
	}
	/* Compute n as a negative number to avoid overflow on LONG_MIN */
	while (isdigit(LLreadchar(s)))
		n = 10*n - ((LLreadchar(s++)) - '0');
	if (!neg) n = -n;
	return n;
}

time_t time(time_t *t)
{
	struct timespec ts;
	__syscall2(__NR_clock_gettime, CLOCK_REALTIME, (long long)&ts);
	if (t) *t = ts.tv_sec;
	return ts.tv_sec;
}

INT __libc_start_main(
	INT (*main)(INT, char **, char **), INT argc, char **argv,
	INT (*init)(INT, char **, char **), void (*fini)(void),
	void (*ldso_fini)(void))
{
	char **envp = argv+argc+1;

	//__init_libc(envp);

	//libc.ldso_fini = ldso_fini;
	//libc.fini = fini;

	/* Execute constructors (static) linked into the application */
	//if (init) init(argc, argv, envp);

	current_brk = (char*)__syscall1(__NR_brk, 0);

	/* Pass control to to application */
	exit(
		main(argc, argv, NULL)//envp)
	)
	;
	return 0;
}
