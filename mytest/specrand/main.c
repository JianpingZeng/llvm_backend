#include "libc.h"
#include "specrand.h"

#define BASE   1000000000L
#define BASEDIGITS 9

typedef long long INDEXER;
typedef long long SHORT;
typedef double   LONG;
typedef long long INT;

void OutDig(INT dig)
{
      putchar(dig + '0');
}

void PrintVal(INT val) {
			if (val == 0) OutDig(0);
			else while(val>0) {
				OutDig(val%10);
				val -= val%10;
			}
}
void PrintShort(SHORT num)
{
      INT x;
      INT digits[BASEDIGITS + 1];

      for (x = 0; x < BASEDIGITS && num != 0; x++)
      {
            digits[x] = num % 10;
            num /= 10;
      }
      for (x--; x >= 0; x--)
            OutDig(digits[x]);
}

double d1 = 1.5, d2 = 4.0, d3;
long long _main(long long argc, char *argv[])
{
    long long seed;
    long long count, i;

    if (argc < 3)
    {
        puts("Please supply seed and count\n");
        exit(EXIT_FAILURE);
    }

    seed = atol(argv[1]);
    count = atol(argv[2]);

    PrintShort(seed);
    puts("\n");
    PrintShort(count);
    puts("\n");
    //d2 = d1 * seed;
    //d3 = d2 * count;
    //PrintShort(d3);
    //puts("\n");
    
    //printf("seed = %d\ncount = %d\n", seed, count);
    //printf("%%f sequence:\n");
    spec_srand(seed);
    for(i = 0; i < count; i++) {
        //PrintShort((long long)spec_rand());
        PrintShort((spec_rand()*2048000.0)+1);
        puts("\n");
    }
    //printf("%%d sequence (1-2048000):\n");
    //for(i = 0; i < count; i++) {
    //    printf("%d\n", (int)((spec_rand()*2048000)+1));
    //}

    return(0);
}
