#include "cachelab.h"
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

struct globalArgs_t {
	int verbose;     /* -v */
	int s;
	int S;
	int E;
	int b;
	int B;
	char *tracefile;

	int hit;
	int miss;
	int eviction;
} globalArgs;

static const char *optString = "hvs:E:b:t:";

typedef struct {
	unsigned long tag;          /* line tag */
	int counter;           /* lru counter */
} cache_line;
cache_line *cache;

void display_usage();

void init();

void Free();

void simulator();

void query(char t, unsigned long tag, int setNo, int time);

void output_verbosely(char type, int situation);

void display_cache_usage();

int main(int argc, char **argv)
{
	int opt = 0;

	/* Initialize globalArgs */
	globalArgs.verbose = 0;
	globalArgs.s = 1;
	globalArgs.S = 2;
	globalArgs.E = 1;
	globalArgs.b = 0;
	globalArgs.b = 1;
	globalArgs.tracefile = NULL;
	globalArgs.hit = 0;
	globalArgs.miss = 0;
	globalArgs.eviction = 0;

	opt = getopt(argc, argv, optString);
	while(-1 != opt)
	{
		switch(opt) 
		{
			case 'h':
				display_usage();
				exit(0);
				break;

			case 'v':
				globalArgs.verbose++;
				break;
			
			case 's':
				globalArgs.s = atoi(optarg);
				globalArgs.S = (1 << globalArgs.s);
				break;

			case 'E':
				globalArgs.E = atoi(optarg);
				break;

			case 'b':
				globalArgs.b = atoi(optarg);
				globalArgs.B = (1 << globalArgs.b);
				break;

			case 't':
				globalArgs.tracefile = optarg;
				break;

			default:
				exit(1);
				break;
		}

		opt = getopt(argc, argv, optString);
	}

	init();

	simulator();

    printSummary(globalArgs.hit, globalArgs.miss, globalArgs.eviction);

	Free();

    return 0;
}

void display_usage()
{
	printf("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
	printf("    -h:Optional help flag that prints usage info\n");
	printf("    -v:Optional verbose flag that displays trace info\n");
	printf("    -s <s>:Number of set index bits (S = 2^s is the number of sets)\n");
	printf("    -E <E>: Associativity (number of lines per set)\n");
	printf("    -b <b>: Number of block bits (B = 2^b is the block size)\n");
	printf("    -t <tracefile>: Name of the valgrind trace to replay\n");
}

void init()
{
	int s = globalArgs.s;
	int E = globalArgs.E;

	cache = (cache_line *)malloc(sizeof(cache_line) * (E << s));
	if(!cache)
	{
		fprintf(stderr, "malloc fail");
	}

	memset(cache, 0, sizeof(cache_line) * (E << s));
}

void simulator()
{
	int time = 0;
	int b = globalArgs.b;
	int s = globalArgs.s;
	int S = globalArgs.S;

	char t;
	unsigned long addr;
	unsigned size;

	int setNo;
	unsigned long tag;

	FILE *file = fopen(globalArgs.tracefile, "r");
	while(3 == fscanf(file, " %c %lx,%d", &t, &addr, &size))
	{
		if(globalArgs.verbose)
		  printf("%c %lx,%d ", t, addr, size);
		setNo = (addr >> b) % S; 
		tag = addr >> (b + s);
		switch (t)
		{
			case 'M': globalArgs.hit++;
			case 'L': 
			case 'S': query(t, tag, setNo, ++time);

		}

	}

	fclose(file);
}

void query(char type, unsigned long tag, int setNo, int time)
{
	//display_cache_usage();
	int E = globalArgs.E;

	cache_line *pline = cache + (E * setNo);
	cache_line *last = pline + E;
	cache_line *evict_line = pline;

	for(; pline != last ; ++pline)
	{
		if(pline->counter > 0 && pline->tag == tag)
		{
			pline->counter = time;
			globalArgs.hit++;
			if(globalArgs.verbose) output_verbosely(type, 0);
			return;
		}
		else if(!pline->counter)
		{
			pline->counter = time;
			pline->tag = tag;
			globalArgs.miss++;
			if(globalArgs.verbose) output_verbosely(type, 1);
			return;
		}
		else if(pline->counter < evict_line->counter)
		{
			evict_line = pline;
		}

	}

	globalArgs.miss++;
	globalArgs.eviction++;
	evict_line->counter = time;
	evict_line->tag = tag;
	if(globalArgs.verbose) output_verbosely(type, 2);
	return;
}

void output_verbosely(char type, int situation)
{
	switch(situation)
	{
		case 0:
			switch(type)
			{
				case 'L':
				case 'S': printf("hit\n"); break;
				case 'M': printf("hit hit\n"); break;
			} break;
		case 1:
			switch(type)
			{
				case 'L':
				case 'S': printf("miss\n"); break;
				case 'M': printf("miss hit\n"); break;
			} break;
		case 2:
			switch(type)
			{
				case 'L':
				case 'S': printf("miss eviction\n"); break;
				case 'M': printf("miss evition hit\n"); break;
			} break;
	}

}

void display_cache_usage()
{
	int E = globalArgs.E;
	int S = globalArgs.S;
	cache_line *pline;
	cache_line *setlast;
	printf("\n");
	for(int i = 0; i < S; ++i)
	{
		pline = cache + (E * i);
		setlast = pline + E;

		printf("set %d: ", i);
		for(; pline != setlast ; ++pline)
		{
			printf("tag(%lx)  counter(%d)  ", pline->tag, pline->counter);
		}
		printf("\n");
	}
}

void Free()
{
	free(cache);
}
