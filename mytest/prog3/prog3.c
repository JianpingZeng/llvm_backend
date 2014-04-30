/* +++Date last modified: 01-Oct-1996 */

/*
  pi8.c  Sept 9, 1996.

  This program is placed into the public domain by its author,
  Carey Bloodworth.

  This pi program can calculate pi with either integers, or doubles.
  It can also use a variety of formulas.

  This code isn't really optimized because it has to work with both the
  long integer version and the double version, and several formulas.
  Compromises had to be made in several places.

  When compiling, you can chose to use the FPU or the integer version.
  By default, it will chose to work only in integers.  If you want to
  use the FPU, define:

#define USEFPU     1

  You have a choice of formulas.  By default, it will use the Machin
  formula of:  pi=16arctan(1/5)-4arctan(1/239)

  You could chose to use one of the other formulas.

  for pi=8arctan(1/3)+4arctan(1/7)
#define ARC3  1
  for pi=12arctan(1/4)+4arctan(1/20)+4arctan(1/1985)
#define ARC4  1
  for pi=16arctan(1/5)-4arctan(1/70)+4arctan(1/99)
#define ARC5  1
  for pi=32arctan(1/10)-4arctan(1/239)-16arctan(1/515)
#define ARC10 1

  Or, of course, you could define it on the compile command line with
  the -D switch.

  Timings were done on a Cyrix 486/66, with the slow Turbo C++ v3.0
          1,000 2,000 3,000 4,000  1,000F 2,000F 3,000F 4,000F
  Machin      4    18    45    86      1      5     11     20
  Arc3        6    29    74   140      2      9     20     35
  Arc4        5    24    64   116      2      7     16     29
  Arc5        6    26    65   123      1      6     15     26
  Arc10       4    19    46    86      1      5     11     19

  All of the combinations above were verified to their indicated
  precision and in each case, only the last few digits were wrong,
  which is a normal round off / truncation error.

  Better compilers will of course result in faster computations,
  but the ratios should be the same.  When I used GCC 2.6.3 for
  DOS, I computed 4,000 digits with with the Machin formula and
  the FPU in 8 seconds.  The integer version took 17 seconds.

  I also used the FPU GCC version to compute 65,536 digits of
  pi and verified them against the Gutenberg PIMIL10.TXT, and
  only the last 4 digits were incorrect.  The computations took
  33 minutes and 54 seconds.
*/

#include "libc.h"

#define SHOWTIME

#if defined USEFPU

#define BASE   1000000000L
#define BASEDIGITS 9

typedef long long SHORT;
typedef double   LONG;
#else

#define BASE   100
#define BASEDIGITS 2

typedef long long SHORT;
typedef long long      LONG;
#endif

typedef long long INDEXER;
typedef long long INT;

SHORT pi[100], powers[100];

long long _main(long long argc, char *argv[])
{
	LONG sum;

	sum = powers[0] + 1;
	if (sum > 100) 
      		sum = -99;
	pi[1] = sum;

 	putchar(pi[1] + '0');
	return EXIT_SUCCESS;
}
