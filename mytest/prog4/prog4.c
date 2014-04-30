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

SHORT pi[100], powers[100], term[100];

void OutDig(INT dig){
			putchar(dig + '0');
}

void PrintVal(INT val) {
			if (val > 9) PrintVal(val/10);
			OutDig(val%10);
}

void PrintShort(SHORT num){
			INT x;
			INT digits[BASEDIGITS + 1];
			for (x = 0; x < BASEDIGITS; x++)
			{
						digits[x] = num % 10;
						num /= 10;
			}
			for (x = BASEDIGITS - 1; x >= 0; x--)
						OutDig(digits[x]);
			putchar('\n');
}

long long _main(long long argc, char *argv[]){
		LONG carry;
		carry = 0;
		if (pi[1] < 2)
		{
			carry = 1;
		}
		pi[1] = carry;
		PrintShort(pi[1]);
		return EXIT_SUCCESS;
}
