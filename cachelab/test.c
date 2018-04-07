#include <stdio.h>

int main()
{
	FILE *file = fopen("traces/trans.trace", "r");
	char c;
	unsigned long addr;
	int size;

	while(fscanf(file, " %c %lx,%d", &c, &addr, &size) == 3)
	{
		printf(" %c %p,%d\n", c, addr, size);
	}
	return 0;
}
