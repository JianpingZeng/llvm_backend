#define EXIT_FAILURE 1LL
#define EXIT_SUCCESS 0LL

#define NULL ((void*)0)

typedef long long __time_t;
struct timespec
  {
    __time_t tv_sec;		/* Seconds.  */
    long long tv_nsec;		/* Nanoseconds.  */
  };
#define CLOCK_REALTIME		0

typedef long long time_t;
typedef unsigned long long size_t;
size_t llstrlen(const char *s);
#define strlen(n) llstrlen(n)
long long isspace(long long c);
long long isdigit(long long c);
long long putchar(long long c);
long long puts(const char *s);
void *malloc(unsigned long size);
long atol(const char *nptr);
time_t time(time_t *t);
void llexit(long long);
#define exit(n) llexit(n)
