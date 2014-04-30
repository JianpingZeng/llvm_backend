#include "syscall.h"

// from musl

long __libc_start_main(
	long (*main)(long, char **, char **), long argc, char **argv,
	long (*init)(long, char **, char **), void (*fini)(void),
	void (*ldso_fini)(void))
{
	char **envp = argv+argc+1;

	//__init_libc(envp);

	//libc.ldso_fini = ldso_fini;
	//libc.fini = fini;

	/* Execute constructors (static) linked into the application */
	//if (init) init(argc, argv, envp);

	/* Pass control to to application */
	__syscall1(__NR_exit,
		main(argc, argv, envp)
	);
	return 0;
}
